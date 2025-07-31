#pragma once

#include "CauldronZero/Macros.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <filesystem> // 需要包含此头文件以使用 std::filesystem::exists

#include <nlohmann/json_fwd.hpp> // Use the official forward-declaration header.

#include "CauldronZero/reflection/Reflection.h"
#include "CauldronZero/reflection/Serialization.h"
#include "CauldronZero/reflection/Deserialization.h"

namespace CauldronZero
{
    // Concept: 检查一个类型是否是有效的配置结构体
    // 要求：可反射，且有一个名为 'version' 的整型成员
    template <typename T>
    concept IsConfig = reflection::Reflectable<T> && requires(T obj) {
        { obj.version } -> std::integral;
    };

	class CZ_API Config
	{
	public:
		Config();

		/**
		 * @brief 从一个 nlohmann::json 对象构造 Config。
		 * @param initial_data 用于初始化配置的 JSON 对象。
		 */
		explicit Config(const nlohmann::json& initial_data);

		/**
		 * @brief 从一个 JSON 格式的字符串构造 Config。
		 * @param json_string 包含有效 JSON 数据的字符串。如果解析失败，将创建一个空的配置。
		 */
		explicit Config(const std::string& json_string);

		~Config(); // 在 .cpp 文件中定义

		/**
		 * @brief 从指定路径加载配置文件。
		 *
		 * @param path 要加载的配置文件的路径。
		 * @return 如果加载成功，则返回 true；否则返回 false。
		 */
		bool load(const std::string& path);

        /**
         * @brief 从文件加载、更新并反序列化一个可反射的配置结构体。
         *
         * @tparam T 配置结构体的类型，必须满足 IsConfig concept。
         * @param config_obj 要填充的配置对象。
         * @param path 配置文件的路径。
         * @param updater 一个可选的函数，用于在版本不匹配时自定义更新逻辑。
         * @return 如果文件被创建或更新，则返回 false；如果文件无需修改，则返回 true。
         */
        template <IsConfig T>
        bool load(T& config_obj, const std::string& path, std::function<bool(T&, nlohmann::json&)> updater = nullptr);

		/**
		 * @brief 将当前配置保存到文件。
		 *
		 * 如果提供了路径，则保存到指定文件。否则，保存到最初加载的文件。
		 *
		 * @param path （可选）要保存配置文件的路径。
		 * @return 如果保存成功，则返回 true；否则返回 false。
		 */
		bool save(const std::string& path = "");

        /**
         * @brief 将一个可反射的配置结构体序列化并保存到文件。
         *
         * @tparam T 配置结构体的类型，必须满足 Reflectable concept。
         * @param config_obj 要保存的配置对象。
         * @param path 要保存到的文件路径。
         * @return 如果保存成功，则返回 true；否则返回 false。
         */
        template <reflection::Reflectable T>
        bool save(const T& config_obj, const std::string& path);

		/**
		 * @brief 将另一个配置对象合并到当前配置中（破坏性合并）。
		 *
		 * 使用 `merge_patch` 语义：
		 * - `other` 中的键值会覆盖当前配置中的同名键值。
		 * - 如果 `other` 中的键值为 `null`，当前配置中对应的键将被删除。
		 * - 合并是递归的。
		 * @param other 要合并进来的配置对象。
		 */
		void merge(const Config& other);

		/**
		 * @brief 使用另一个配置对象更新当前配置（非破坏性更新）。
		 *
		 * 使用 `update` 语义：
		 * - `other` 中的键值会覆盖当前配置中的同名键值。
		 * - `other` 中的 `null` 值也会被正常合并，而不会删除键。
		 * - 合并是递归的。
		 * @param other 用于更新的配置对象。
		 */
		void update(const Config& other);

		/**
		 * @brief 启动一个后台线程来监视配置文件的更改。
		 *
		 * 当检测到文件被修改时，会自动重新加载配置。
		 *
		 * @param callback （可选）一个在配置成功重载后调用的回调函数。
		 */
		void watch(std::function<void()> callback = nullptr);

		/**
		 * @brief 停止监视配置文件。
		 */
		void unwatch();

		/**
		 * @brief 获取指定路径的配置值。
		 *
		 * @tparam T 期望的返回类型。
		 * @param path 配置路径，使用点（.）分隔。
		 * @return 如果路径存在且类型匹配，则返回包含值的 std::optional；否则返回 std::nullopt。
		 *         当路径格式无效、路径不存在或类型不匹配时，会记录详细的错误日志。
		 */
		template<typename T>
		std::optional<T> get(const std::string& path) const;

		/**
		 * @brief 获取指定路径的配置值，如果不存在则返回默认值。
		 *
		 * @tparam T 期望的返回类型。
		 * @param path 配置路径，使用点（.）分隔。
		 * @param defaultValue 如果路径不存在或类型不匹配，则返回此默认值。
		 * @return 返回获取到的值或默认值。
		 */
		template<typename T>
		T get(const std::string& path, T defaultValue) const;

		/**
		 * @brief 设置指定路径的配置值。
		 *
		 * @tparam T 要设置的值的类型。
		 * @param path 配置路径，使用点（.）分隔。
		 * @param value 要设置的值。
		 */
		template<typename T>
		void set(const std::string& path, const T& value);

		/**
		 * @brief 获取底层的 nlohmann::json 对象（只读）。
		 *
		 * 这对于需要遍历配置或执行 nlohmann::json 库特定操作的高级用例非常有用。
		 *
		 * @return 对底层 json 对象的常量引用。
		 */
		const nlohmann::json& get_json() const;

		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) noexcept;
		Config& operator=(Config&&) noexcept;

	private:
		class Impl; // 前向声明 PImpl
		std::unique_ptr<Impl> pimpl;

        // 默认的配置更新器
        template <IsConfig T>
        static bool defaultConfigUpdater(T& config, nlohmann::json& data);
	};

    // --- 模板函数实现 ---

    template <IsConfig T>
    bool Config::defaultConfigUpdater(T& config, nlohmann::json& data)
    {
        // 移除旧版本号，以便使用新结构体的版本号
        if (data.contains("version")) {
            data.erase("version");
        }

        // 将默认配置序列化为 JSON
        auto patch = reflection::serialize(config);
        if (!patch) {
            return false; // 序列化失败
        }

        // 将磁盘上的旧配置覆盖到默认配置上
        patch->merge_patch(data);

        // 更新 data 为合并后的结果
        data = std::move(*patch);
        return true;
    }

    template <IsConfig T>
    bool Config::load(T& config_obj, const std::string& path, std::function<bool(T&, nlohmann::json&)> updater)
    {
        bool noNeedRewrite = true;
        if (!std::filesystem::exists(path)) {
            // 如果文件不存在，直接保存默认配置
            save(config_obj, path);
            noNeedRewrite = false;
        }

        // 从文件加载原始 JSON 数据
        Config file_config;
        if (!file_config.load(path)) {
            // 如果加载失败（例如文件损坏），也用默认配置覆盖
            save(config_obj, path);
            return false;
        }

        nlohmann::json data = file_config.get_json();

        // 检查版本
        if (!data.contains("version") || !data["version"].is_number() || data["version"].get<int>() != config_obj.version)
        {
            noNeedRewrite = false;
            // 版本不匹配，调用更新器
            auto do_update = updater ? updater : defaultConfigUpdater<T>;
            if (!do_update(config_obj, data)) {
                // 更新失败，记录错误
                // logger.error("Failed to update config structure for '{}'", path);
                return false;
            }
            // 更新后需要重写文件
            Config updated_config(data);
            updated_config.save(path);
        }

        // 将最终的 JSON 数据反序列化到配置对象中
        reflection::deserialize(config_obj, data);

        return noNeedRewrite;
    }

    template <reflection::Reflectable T>
    bool Config::save(const T& config_obj, const std::string& path)
    {
        auto json_opt = reflection::serialize(config_obj);
        if (!json_opt) {
            return false; // 序列化失败
        }

        Config config_to_save(*json_opt);
        return config_to_save.save(path);
    }
}
