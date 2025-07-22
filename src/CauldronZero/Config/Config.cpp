#include <nlohmann/json.hpp> // 必须在 Config.h 之前包含，以解决前向声明问题
#include "Config.h"
#include "CauldronZero/logger.h"
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <regex>
#include <shared_mutex>
#include <thread>
#include <typeinfo>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <algorithm> // For std::replace
#endif

namespace
{
#ifdef _WIN32
    // 辅助函数：根据文件路径创建一个唯一的互斥体名称，用于跨进程同步。
    std::wstring create_mutex_name(const std::string& path)
    {
        std::string sanitized_path = path;
        // 将反斜杠替换为中性字符，以创建有效的互斥体名称。
        std::replace(sanitized_path.begin(), sanitized_path.end(), '\\', '_');
        std::replace(sanitized_path.begin(), sanitized_path.end(), '/', '_');
        std::replace(sanitized_path.begin(), sanitized_path.end(), ':', '_');

        // 转换为 Windows API 使用的 wstring
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, sanitized_path.c_str(), (int)sanitized_path.size(), nullptr, 0);
        std::wstring wpath(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, sanitized_path.c_str(), (int)sanitized_path.size(), &wpath[0], size_needed);

        return L"Global\\CauldronZero_Config_" + wpath;
    }
#endif
    // 辅助函数：获取用户友好的类型名称
    template <typename T>
    const char* get_type_name()
    {
        if constexpr (std::is_same_v<T, std::string>) return "string";
        if constexpr (std::is_same_v<T, int>) return "int";
        if constexpr (std::is_same_v<T, bool>) return "bool";
        if constexpr (std::is_same_v<T, double>) return "double";
        if constexpr (std::is_same_v<T, unsigned int>) return "unsigned int";
        if constexpr (std::is_same_v<T, uint64_t>) return "uint64_t";
        if constexpr (std::is_same_v<T, std::vector<std::string>>) return "array of strings";
        if constexpr (std::is_same_v<T, std::vector<int>>) return "array of ints";
        if constexpr (std::is_same_v<T, std::vector<bool>>) return "array of bools";
        if constexpr (std::is_same_v<T, std::vector<double>>) return "array of doubles";
        if constexpr (std::is_same_v<T, nlohmann::json>) return "json object";
        return typeid(T).name(); // 其他类型的备用方案
    }
} // namespace

namespace CauldronZero
{
    // PImpl idiom: 隐藏实现细节
    class Config::Impl
    {
    public:
        nlohmann::json data;
        std::string original_path;
        mutable std::shared_mutex mutex; // 用于进程内线程安全

#ifdef _WIN32
        HANDLE hMutex = nullptr; // 用于跨进程同步
#endif

        // 用于文件监视（热重载）
        std::thread watcher_thread;
        std::atomic<bool> watching = false;
        std::filesystem::file_time_type last_write_time;
        std::function<void()> reload_callback = nullptr;

        void stop_watching()
        {
            if (watching.exchange(false)) {
                if (watcher_thread.joinable()) {
                    watcher_thread.join();
                }
            }
        }
    };

    Config::Config() : pimpl(std::make_unique<Impl>())
    {
        pimpl->data = nlohmann::json::object();
    }

    Config::Config(const nlohmann::json& initial_data) : pimpl(std::make_unique<Impl>())
    {
        pimpl->data = initial_data;
    }

    Config::Config(const std::string& json_string) : pimpl(std::make_unique<Impl>())
    {
        try {
            pimpl->data = nlohmann::json::parse(json_string, nullptr, true, true);
        } catch (const nlohmann::json::parse_error& e) {
            logger.error("Failed to parse JSON string to create config. Reason: {}", e.what());
            pimpl->data = nlohmann::json::object(); // 解析失败时初始化为空对象
        }
    }

    Config::~Config()
    {
        if (pimpl) {
            pimpl->stop_watching();
#ifdef _WIN32
            if (pimpl->hMutex) {
                CloseHandle(pimpl->hMutex);
            }
#endif
        }
    }

    Config::Config(Config&& other) noexcept : pimpl(std::move(other.pimpl)) {}

    Config& Config::operator=(Config&& other) noexcept
    {
        if (this != &other)
        {
            pimpl = std::move(other.pimpl);
        }
        return *this;
    }

    bool Config::load(const std::string& path)
    {
#ifdef _WIN32
        if (!pimpl->hMutex) {
            auto mutex_name = create_mutex_name(path);
            pimpl->hMutex = CreateMutexW(nullptr, FALSE, mutex_name.c_str());
        }

        if (!pimpl->hMutex) {
            logger.error("Failed to create system-wide mutex for config file: {}", path);
            return false;
        }

        DWORD waitResult = WaitForSingleObject(pimpl->hMutex, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            logger.error("Failed to lock system-wide mutex for config file: {}", path);
            return false;
        }
#endif

        // Defer releasing the mutex until the end of the scope
        auto release_guard = std::unique_ptr<void, decltype(&ReleaseMutex)>(pimpl->hMutex, &ReleaseMutex);

        std::unique_lock lock(pimpl->mutex);
        std::ifstream file(path);
        if (!file.is_open()) {
            logger.error("Failed to open config file: {}", path);
            return false;
        }

        try {
            // nlohmann/json 库本身支持忽略注释
            pimpl->data = nlohmann::json::parse(file, nullptr, true, true);
            pimpl->original_path = path; // 保存原始路径
            try {
                pimpl->last_write_time = std::filesystem::last_write_time(path);
            } catch (const std::filesystem::filesystem_error& fs_err) {
                logger.warn("Could not read last write time for {}: {}", path, fs_err.what());
            }
            logger.info("Config file loaded successfully: {}", path);
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            logger.error("Failed to parse config file: {}. Reason: {}", path, e.what());
            return false;
        }
    }

    void Config::merge(const Config& other)
    {
        if (!pimpl || !other.pimpl) {
            return;
        }

        std::unique_lock lock(pimpl->mutex);
        std::shared_lock other_lock(other.pimpl->mutex);

        if (pimpl->data.is_object() && other.pimpl->data.is_object()) {
            pimpl->data.merge_patch(other.pimpl->data);
        }
    }

    void Config::update(const Config& other)
    {
        if (!pimpl || !other.pimpl) {
            return;
        }

        std::unique_lock lock(pimpl->mutex);
        std::shared_lock other_lock(other.pimpl->mutex);

        if (pimpl->data.is_object() && other.pimpl->data.is_object()) {
            pimpl->data.update(other.pimpl->data);
        }
    }

    void Config::watch(std::function<void()> callback)
    {
        if (!pimpl || pimpl->original_path.empty()) {
            logger.error("Cannot watch a config that was not loaded from a file.");
            return;
        }

        if (pimpl->watching.exchange(true)) {
            logger.warn("Config watcher is already running.");
            return;
        }

        pimpl->reload_callback = std::move(callback);

        pimpl->watcher_thread = std::thread([this]() {
            while (pimpl->watching) {
                try {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    if (!pimpl->watching) break;

                    auto current_write_time = std::filesystem::last_write_time(pimpl->original_path);

                    if (current_write_time > pimpl->last_write_time) {
                        logger.info("Config file '{}' has changed, attempting to reload.", pimpl->original_path);
                        if (load(pimpl->original_path)) {
                            // load 函数在成功时会更新 last_write_time
                            if (pimpl->reload_callback) {
                                pimpl->reload_callback();
                            }
                        }
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    logger.error("Error checking file status for '{}': {}", pimpl->original_path, e.what());
                    // 如果文件不再可访问，则停止监视
                    pimpl->watching = false;
                } catch (const std::exception& e) {
                    logger.error("An unexpected error occurred in the config watcher thread: {}", e.what());
                }
            }
        });
    }

    void Config::unwatch()
    {
        if (pimpl) {
            pimpl->stop_watching();
        }
    }

    bool Config::save(const std::string& path)
    {
        if (!pimpl) {
            return false;
        }

        std::string save_path = path.empty() ? pimpl->original_path : path;
        if (save_path.empty()) {
            logger.error("No path specified for saving config, and no original path was loaded.");
            return false;
        }

#ifdef _WIN32
        if (!pimpl->hMutex) {
            logger.error("Cannot save config that was not loaded first (system mutex not initialized).");
            return false;
        }

        DWORD waitResult = WaitForSingleObject(pimpl->hMutex, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            logger.error("Failed to lock system-wide mutex for config file: {}", save_path);
            return false;
        }
#endif
        // Defer releasing the mutex until the end of the scope
        auto release_guard = std::unique_ptr<void, decltype(&ReleaseMutex)>(pimpl->hMutex, &ReleaseMutex);

        // Use a unique lock as we might merge data back into pimpl->data
        std::unique_lock lock(pimpl->mutex);

        try {
            // “读取-合并-写入”以防止其他进程造成数据丢失
            nlohmann::json disk_data;
            std::ifstream infile(save_path);
            if (infile.is_open()) {
                // 忽略读取时的解析错误，因为我们即将覆盖它。
                // 这可以处理文件为空或损坏的情况。
                disk_data = nlohmann::json::parse(infile, nullptr, false);
            }
            infile.close();

            // 将内存中的更改合并到磁盘上的数据中
            if (disk_data.is_object() && pimpl->data.is_object()) {
                disk_data.merge_patch(pimpl->data);
            } else {
                // 如果类型不是对象或不匹配，则以我们的版本为准。
                disk_data = pimpl->data;
            }

            // 将合并后的数据写回文件
            std::ofstream outfile(save_path);
            if (!outfile.is_open()) {
                logger.error("Failed to open file for writing: {}", save_path);
                return false;
            }

            outfile << disk_data.dump(4); // 4空格缩进
            outfile.close();

            // 更新我们的内存视图，使其与刚保存的内容保持一致
            pimpl->data = disk_data;
            pimpl->original_path = save_path; // 保存成功后，更新原始路径
            try {
                pimpl->last_write_time = std::filesystem::last_write_time(save_path);
            } catch (const std::filesystem::filesystem_error& fs_err) {
                logger.warn("Could not read last write time for {}: {}", save_path, fs_err.what());
            }

            logger.info("Config saved successfully to: {}", save_path);
            return true;
        } catch (const std::exception& e) {
            logger.error("Failed to serialize or save config to JSON. Reason: {}", e.what());
            return false;
        }
    }

    // 辅助函数，用于递归解析路径，优先匹配更长的键。
    // 示例："a.b.c" 会首先查找键 "a.b"，然后是 "a"。
    static const nlohmann::json* resolve_path(const nlohmann::json& root, const std::string& path)
    {
        if (!root.is_object()) {
            return nullptr;
        }

        // 贪婪地查找最长匹配的键前缀。
        for (size_t i = path.rfind('.'); i != std::string::npos; i = (i > 0) ? path.rfind('.', i - 1) : std::string::npos) {
            std::string head = path.substr(0, i);
            if (root.contains(head)) {
                const nlohmann::json& next_node = root.at(head);
                if (next_node.is_object()) {
                    std::string tail = path.substr(i + 1);
                    const nlohmann::json* result = resolve_path(next_node, tail);
                    if (result) {
                        return result;
                    }
                }
            }
            if (i == 0) {
                break;
            }
        }

        // 如果没有前缀匹配成功，则尝试将整个路径作为一个键。
        if (root.contains(path)) {
            return &root.at(path);
        }

        return nullptr;
    }

    template<typename T>
    std::optional<T> Config::get(const std::string& path) const
    {
        if (!pimpl) {
            return std::nullopt;
        }

        std::shared_lock lock(pimpl->mutex);

        const nlohmann::json* value_node = resolve_path(pimpl->data, path);

        if (!value_node) {
            logger.debug("Config path '{}' not found.", path);
            return std::nullopt;
        }

        try {
            return value_node->get<T>();
        } catch (const nlohmann::json::type_error& e) {
            logger.warn("Config type mismatch for path '{}'. Expected type '{}' but got JSON type '{}'.",
                        path,
                        get_type_name<T>(),
                        value_node->type_name());
            logger.debug("Details: {}", e.what());
            return std::nullopt;
        } catch (const std::exception& e) {
            logger.error("Unexpected error while getting config path '{}': {}", path, e.what());
            return std::nullopt;
        }
    }

    template<typename T>
    T Config::get(const std::string& path, T defaultValue) const
    {
        auto value = get<T>(path);
        return value.value_or(defaultValue);
    }

    // 辅助函数，用于解析写入路径。如果嵌套对象不存在，则会创建它们。
    // 它遵循与 `get` 相同的“最长前缀”逻辑进行遍历，
    // 但如果路径无法解析，则回退到创建一个简单的嵌套结构。
    static nlohmann::json* resolve_or_create_path(nlohmann::json& root, const std::string& path)
    {
        if (!root.is_object()) {
            return nullptr; // 无法遍历非对象类型
        }

        // 贪婪地查找作为对象的最长匹配键前缀。
        for (size_t i = path.rfind('.'); i != std::string::npos; i = (i > 0) ? path.rfind('.', i - 1) : std::string::npos) {
            std::string head = path.substr(0, i);
            if (root.contains(head)) {
                nlohmann::json& next_node = root.at(head);
                if (next_node.is_object()) {
                    std::string tail = path.substr(i + 1);
                    return resolve_or_create_path(next_node, tail);
                }
            }
            if (i == 0) {
                break;
            }
        }

        // 如果未找到现有对象路径，则通过在每个点处拆分来创建路径。
        // 这保留了创建新设置时的原始行为。
        nlohmann::json* current = &root;
        size_t start = 0;
        while (true) {
            size_t end = path.find('.', start);
            std::string key = path.substr(start, end - start);

            if (end == std::string::npos) {
                // 这是路径中的最后一个键。
                return &(*current)[key];
            }

            nlohmann::json& next_node = (*current)[key];
            if (!next_node.is_object()) {
                // 路径中有非对象类型阻挡。覆盖它以继续创建路径。
                next_node = nlohmann::json::object();
            }
            current = &next_node;
            start = end + 1;
        }
    }

    template<typename T>
    void Config::set(const std::string& path, const T& value)
    {
        if (!pimpl) {
            return;
        }

        std::unique_lock lock(pimpl->mutex);

        try {
            nlohmann::json* target_node = resolve_or_create_path(pimpl->data, path);
            if (target_node) {
                *target_node = value;
            } else {
                logger.error("Failed to set config for path '{}' because root is not an object.", path);
            }
        } catch (const std::exception& e) {
            logger.error("Failed to set config for path '{}'. Reason: {}", path, e.what());
        }
    }

    // 显式实例化模板
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<unsigned int> Config::get<unsigned int>(const std::string&) const;
    template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
    template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
    template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
    template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;
    template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
    template std::optional<nlohmann::json> Config::get<nlohmann::json>(const std::string&) const;

    template void Config::set<std::string>(const std::string&, const std::string&);
    template void Config::set<int>(const std::string&, const int&);
    template void Config::set<bool>(const std::string&, const bool&);
    template void Config::set<double>(const std::string&, const double&);
    template void Config::set<unsigned int>(const std::string&, const unsigned int&);
    template void Config::set<uint64_t>(const std::string&, const uint64_t&);
    template void Config::set<std::vector<std::string>>(const std::string&, const std::vector<std::string>&);
    template void Config::set<std::vector<int>>(const std::string&, const std::vector<int>&);
    template void Config::set<std::vector<bool>>(const std::string&, const std::vector<bool>&);
    template void Config::set<std::vector<double>>(const std::string&, const std::vector<double>&);
    template void Config::set<nlohmann::json>(const std::string&, const nlohmann::json&);

    template std::string Config::get<std::string>(const std::string&, std::string) const;
    template int Config::get<int>(const std::string&, int) const;
    template bool Config::get<bool>(const std::string&, bool) const;
    template double Config::get<double>(const std::string&, double) const;
    template unsigned int Config::get<unsigned int>(const std::string&, unsigned int) const;
    template uint64_t Config::get<uint64_t>(const std::string&, uint64_t) const;
    template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&, std::vector<std::string>) const;
    template std::vector<int> Config::get<std::vector<int>>(const std::string&, std::vector<int>) const;
    template std::vector<bool> Config::get<std::vector<bool>>(const std::string&, std::vector<bool>) const;
    template std::vector<double> Config::get<std::vector<double>>(const std::string&, std::vector<double>) const;
    template nlohmann::json Config::get<nlohmann::json>(const std::string&, nlohmann::json) const;
}
