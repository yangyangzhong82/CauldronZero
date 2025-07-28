#pragma once

#include "CauldronZero/Macros.h"
#include <any>
#include <functional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace CauldronZero::Utils {

class PlaceholderManager {
public:
    // 回调函数，用于根据上下文生成占位符的值
    using PlaceholderCallback = std::function<std::string(const std::any&)>;

    CZ_API static PlaceholderManager& getInstance();

    // 禁止拷贝和赋值
    PlaceholderManager(const PlaceholderManager&)            = delete;
    PlaceholderManager& operator=(const PlaceholderManager&) = delete;

    /**
     * @brief 注册一个占位符。
     * @param key 占位符的键（不需要包含 '{}'）。例如："username", "date"。
     * @param callback 用于生成替换值的回调函数。
     * @return 如果键是新的并且成功注册，则为 true；如果键已存在，则为 false。
     */
    CZ_API bool registerPlaceholder(const std::string& key, PlaceholderCallback callback);

    /**
     * @brief 注销一个占位符。
     * @param key 要注销的键。
     * @return 如果键存在并被成功移除，则为 true。
     */
    CZ_API bool unregisterPlaceholder(const std::string& key);

    /**
     * @brief 在文本中查找并替换所有格式为 "{key}" 的占位符。
     * @param text 包含占位符的原始文本。
     * @param context 传递给回调函数的可选上下文。
     * @return 替换占位符后的新字符串。
     */
    CZ_API std::string replacePlaceholders(const std::string& text, const std::any& context = {});

    /**
     * @brief 获取所有已注册的占位符键。
     * @return 一个包含所有键的向量。
     */
    CZ_API std::vector<std::string> getRegisteredPlaceholders() const;

private:
    PlaceholderManager()  = default;
    ~PlaceholderManager() = default;

    mutable std::shared_mutex                            mMutex;
    std::unordered_map<std::string, PlaceholderCallback> mPlaceholders;
};

} // namespace CauldronZero::Utils