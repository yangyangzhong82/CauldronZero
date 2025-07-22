#pragma once

#include "CauldronZero/Macros.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

#include <nlohmann/json_fwd.hpp> // Use the official forward-declaration header.

namespace CauldronZero
{
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
		 * @brief 将当前配置保存到文件。
		 *
		 * 如果提供了路径，则保存到指定文件。否则，保存到最初加载的文件。
		 *
		 * @param path （可选）要保存配置文件的路径。
		 * @return 如果保存成功，则返回 true；否则返回 false。
		 */
		bool save(const std::string& path = "");

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

		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) noexcept;
		Config& operator=(Config&&) noexcept;

	private:
		class Impl; // 前向声明 PImpl
		std::unique_ptr<Impl> pimpl;
	};
}
