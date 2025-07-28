
#include "PlaceholderManager.h"
#include <regex>

namespace CauldronZero::Utils {

PlaceholderManager& PlaceholderManager::getInstance() {
    static PlaceholderManager instance;
    return instance;
}

// 注册时，用户提供的是纯粹的 key，例如 "username"
bool PlaceholderManager::registerPlaceholder(const std::string& key, PlaceholderCallback callback) {
    if (key.empty() || key.find_first_of("{}") != std::string::npos) {
        // 拒绝包含特殊字符的 key，避免混淆
        return false;
    }

    std::unique_lock lock(mMutex);
    if (mPlaceholders.count(key)) {
        return false; // Key already exists
    }
    mPlaceholders[key] = std::move(callback);
    return true;
}

bool PlaceholderManager::unregisterPlaceholder(const std::string& key) {
    std::unique_lock lock(mMutex);
    return mPlaceholders.erase(key) > 0;
}

std::string PlaceholderManager::replacePlaceholders(const std::string& text, const std::any& context) {
    // 正则表达式，用于匹配 "{key}" 格式的占位符
    // \{      -> 匹配字面量 '{'
    // ([^}]+) -> 捕获组1：匹配一个或多个非 '}' 的字符。这就是我们的 key
    // \}      -> 匹配字面量 '}'
    static const std::regex placeholderRegex(R"(\{([^}]+)\})");

    std::string result;
    result.reserve(text.length()); // 为提高性能，预先分配内存

    auto it      = std::sregex_iterator(text.cbegin(), text.cend(), placeholderRegex);
    auto end     = std::sregex_iterator();
    auto last_it = text.cbegin();

    for (; it != end; ++it) {
        const std::smatch& match = *it;
        const std::string  key   = match[1].str(); // 捕获组1是我们的 key

        // 1. 追加上一个匹配到当前匹配之间的文本
        result.append(last_it, match[0].first);

        // 2. 查找 key 并追加替换值
        PlaceholderCallback callback = nullptr;
        {
            std::shared_lock lock(mMutex);
            auto             placeholder_it = mPlaceholders.find(key);
            if (placeholder_it != mPlaceholders.end()) {
                callback = placeholder_it->second;
            }
        }

        if (callback) {
            // 找到了回调函数，调用它并追加结果
            result.append(callback(context));
        } else {
            // 没有找到对应的回调，或者回调为空，则将原始占位符追加回去
            result.append(match[0].first, match[0].second);
        }


        // 3. 更新迭代器位置
        last_it = match[0].second;
    }

    // 4. 追加最后一个匹配到字符串末尾的文本
    result.append(last_it, text.cend());

    return result;
}

std::vector<std::string> PlaceholderManager::getRegisteredPlaceholders() const {
    std::shared_lock         lock(mMutex);
    std::vector<std::string> keys;
    keys.reserve(mPlaceholders.size());
    for (const auto& pair : mPlaceholders) {
        keys.push_back(pair.first);
    }
    return keys;
}

} // namespace CauldronZero::Utils
