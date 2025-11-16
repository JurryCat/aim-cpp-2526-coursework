#include "Song.h"
#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm>
#include <cctype> 
#include <sstream> 
#include <iterator>

//  静态成员变量初始化
int Song::next_id_ = 1;

//  匿名命名空间 (Anonymous Namespace)
namespace {
// @brief 返回去除首尾空白的字符串副本。
std::string trim_copy(const std::string &s) {
    auto first = std::find_if_not(s.begin(), s.end(),
                                  [](const unsigned char ch) { return std::isspace(ch); });
    auto last = std::find_if_not(s.rbegin(), s.rend(),
                                 [](const unsigned char ch) { return std::isspace(ch); })
                    .base();
    if (first >= last)
        return {};
    return {first, last};
}

// @brief 返回字符串的小写副本。
std::string to_lower_copy(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

// @brief 将标签向量（列表）连接成逗号分隔的字符串。
std::string join_tags(const std::vector<std::string> &tags) {
    if (tags.empty()) {
        return "";
    }
    std::stringstream ss;
    for (size_t i = 0; i < tags.size(); ++i) {
        ss << tags[i];
        if (i < tags.size() - 1) {
            ss << ", "; // 在标签之间添加逗号和空格
        }
    }
    return ss.str();
}

} // 结束 匿名命名空间

// 构造函数
Song::Song(const std::string &title,
           const std::string &artist,
           int duration_sec,
           int rating) {
    //  清理输入
    const std::string clean_title = trim_copy(title);
    const std::string clean_artist = trim_copy(artist);
    //  校验数据 (使用 Song.h 中指定的精确字符串)
    if (clean_title.empty()) {
        std::cout << "[错误] 标题不能为空\n";
        return; // 3. 校验失败，中断构造
    }
    if (clean_artist.empty()) {
        std::cout << "[错误] 艺人不能为空\n";
        return;
    }
    if (duration_sec <= 0) {
        std::cout << "[错误] 时长必须为正整数（秒）\n";
        return;
    }
    if (rating < 1 || rating > 5) {
        std::cout << "[错误] 评分必须在 1...5 之间\n";
        return;
    }
    // 所有校验通过
    id_ = next_id_++; // 分配 ID
    title_ = clean_title;
    artist_ = clean_artist;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true; // 标记为有效
}
// 修改器 Setters
bool Song::set_title(const std::string &t) {
    std::string clean_t = trim_copy(t);
    if (clean_t.empty()) {
        std::cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = clean_t;
    return true;
}
bool Song::set_artist(const std::string &a) {
    std::string clean_a = trim_copy(a);
    if (clean_a.empty()) {
        std::cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = clean_a;
    return true;
}
bool Song::set_duration(int sec) {
    if (sec <= 0) {
        std::cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        std::cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// 标签管理 
bool Song::add_tag(const std::string &tag) {
    // 清理
    std::string clean_tag = trim_copy(tag);
    // 检查空
    if (clean_tag.empty()) {
        std::cout << "[提示] 空标签忽略。\n";
        return false;
    }
    // 检查重复（大小写不敏感）
    std::string new_tag_lower = to_lower_copy(clean_tag);
    for (const std::string &existing_tag : tags_) {
        if (to_lower_copy(existing_tag) == new_tag_lower) {
            std::cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }
    // 添加（原始大小写的）清理后标签
    tags_.push_back(clean_tag);
    return true;
}

bool Song::remove_tag(const std::string &tag) {
    // 清理并转换为小写，用于比较
    std::string tag_to_remove_lower = to_lower_copy(trim_copy(tag));
    if (tag_to_remove_lower.empty()) {
        std::cout << "[提示] 未找到该标签\n";
        return false;
    }
    // 遍历 (使用带索引的循环，按 Song.h 提示)
    for (size_t i = 0; i < tags_.size(); ++i) {
        std::string existing_tag_lower = to_lower_copy(tags_[i]);
        // 比较小写版本
        if (existing_tag_lower == tag_to_remove_lower) {
            tags_.erase(tags_.begin() + i); // 找到，移除
            return true;
        }
    }
    // 循环结束都没找到
    std::cout << "[提示] 未找到该标签\n";
    return false;
}
// --- 功能函数 ---
bool Song::matches_keyword(const std::string &kw) const {
    // 清理 kw
    std::string clean_kw = to_lower_copy(trim_copy(kw));
    // 空 kw 不匹配任何东西
    if (clean_kw.empty()) {
        return false;
    }
    // 转换成员变量为小写
    std::string title_lower = to_lower_copy(title_);
    std::string artist_lower = to_lower_copy(artist_);
    // 检查 Title
    if (title_lower.find(clean_kw) != std::string::npos) {
        return true;
    }
    // 检查 Artist
    if (artist_lower.find(clean_kw) != std::string::npos) {
        return true;
    }

    // 检查所有 Tags
    for (const std::string &tag : tags_) {
        if (to_lower_copy(tag).find(clean_kw) != std::string::npos) {
            return true;
        }
    }
    // 都不匹配
    return false;
}
// 友元函数 (操作符重载)

//@brief 重载 << 操作符 (实现)
std::ostream &operator<<(std::ostream &os, const Song &s) {
    // 打印基础信息
    os << "[#" << s.id_ << "] "
       << s.artist_ << " - " << s.title_
       << " (" << s.duration_sec_ << "s)";
    // 打印评分
    // 使用 string 的构造函数 (count, char) 来创建 N 个星号
    os << " " << std::string(s.rating_, '*');
    // 打印标签
    if (!s.tags_.empty()) {
        os << "  [tags: " << join_tags(s.tags_) << "]";
    }
    return os;
}
//@brief 重载 < 操作符 (实现)
//排序逻辑：1. 评分(高->低)  2. 标题(A->Z)  3. ID(小->大)
bool operator<(const Song &a, const Song &b) {
    // 比较评分（降序）
    if (a.rating_ > b.rating_) return true;  // A 评分高，A 排前面
    if (a.rating_ < b.rating_) return false; // B 评分高，B 排前面
    // 评分相等，比较标题（升序）
    if (a.title_ < b.title_) return true;  // A 标题靠前，A 排前面
    if (a.title_ > b.title_) return false; // B 标题靠前，B 排前面
    // 标题相等，比较 ID（升序）
    return a.id_ < b.id_; // A 的 ID 小，A 排前面
}
