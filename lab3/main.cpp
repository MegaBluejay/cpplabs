#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <cmath>

#include "pugixml.hpp"

std::vector<std::string> split(const std::string& s, const std::regex& rgx, bool allow_empty) {
    std::sregex_token_iterator iter (s.begin(), s.end(), rgx, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> res;
    for (; iter != end; ++iter) {
        std::string word = *iter;
        if (!word.empty() || allow_empty) {
            res.push_back(word);
        }
    }
    return res;
}

std::string join(const std::vector<std::string>& ss, const std::string& delim) {
    std::string ans;
    bool fst = true;
    for (const std::string& s : ss) {
        if (fst) {
            fst = false;
        } else {
            ans += delim;
        }
        ans += s;
    }
    return ans;
}

double distance(const std::pair<double, double>& p1, const std::pair<double, double>& p2) {
    static const double R = 6371000;
    auto [lat1, lon1] = p1;
    auto [lat2, lon2] = p2;
    lat1 = lat1*M_PI/180;
    lon1 = lon1*M_PI/180;
    lat2 = lat2*M_PI/180;
    lon2 = lon2*M_PI/180;
    return R*2*std::asin(std::sqrt((1 - std::cos(lat1-lat2))/2 + std::cos(lat1)*std::cos(lat2)*(1 - std::cos(lon1-lon2))/2));
}

std::vector<std::string> get_routes(const std::string& full_routes_string) {
    static const std::regex route_split_rgx (",");
    static const std::regex route_cut_rgx ("\\.");
    std::string routes_string = split(full_routes_string, route_cut_rgx, false)[0];
    return split(routes_string, route_split_rgx, false);
}

template<class K, class V>
bool comp_by_value(const std::pair<K,V>& p1, const std::pair<K,V>& p2) {
    return p1.second < p2.second;
}

std::unordered_map<std::string, std::pair<std::string, int>> task1 (const pugi::xml_node& dataset) {
    std::unordered_map<std::string, std::unordered_map<std::string, int>> map;
    for (pugi::xml_node station = dataset.first_child(); station; station = station.next_sibling()) {
        std::string type = station.child_value("type_of_vehicle");
        std::string full_routes_string = station.child_value("routes");
        std::vector<std::string> routes = get_routes(full_routes_string);
        if (!map.count(type)) {
            map[type] = std::unordered_map<std::string, int> ();
        }
        for (const std::string& route : routes) {
            if (!map[type].count(route)) {
                map[type][route] = 0;
            }
            map[type][route]++;
        }
    }
    std::unordered_map<std::string, std::pair<std::string, int>> ans;
    for (const auto& [type, routes] : map) {
        ans[type] = *std::max_element(routes.begin(), routes.end(), comp_by_value<std::string, int>);
    }
    return ans;
}

double mst_weight(const std::vector<std::pair<double, double>>& points) {
    int n = points.size();
    std::vector<bool> in_set (n, false);
    std::vector<double> keys (n, 1000000);
    keys[0] = 0;
    double ans = 0;
    for (int i = 0; i < n; ++i) {
        double mn = 1000000;
        int u = 0;
        for (int j = 0; j < n; ++j) {
            if (keys[j] < mn && !in_set[j]) {
                mn = keys[j];
                u = j;
            }
        }
        in_set[u] = true;
        ans += keys[u];
        for (int j = 0; j < n; ++j) {
            if (!in_set[j]) {
                double d = distance(points[u], points[j]);
                if (d < keys[j]) {
                    keys[j] = d;
                }
            }
        }
    }
    return ans;
}

std::unordered_map<std::string, std::pair<std::string, double>> task2(const pugi::xml_node& dataset) {
    static const std::regex coord_split_rgx (",");
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::pair<double, double>>>> map;
    for (pugi::xml_node station = dataset.first_child(); station; station = station.next_sibling()) {
        std::string type = station.child_value("type_of_vehicle");
        std::string full_routes_string = station.child_value("routes");
        std::vector<std::string> routes = get_routes(full_routes_string);
        std::string coordinates_string = station.child_value("coordinates");
        std::vector coords_strings = split(coordinates_string, coord_split_rgx, false);
        std::pair<double, double> coords = std::make_pair(std::stod(coords_strings[0]), std::stod(coords_strings[1]));

        if (!map.count(type)) {
            map[type] = std::unordered_map<std::string, std::vector<std::pair<double, double>>> ();
        }
        for (const std::string& route : routes) {
            if (!map[type].count(route)) {
                map[type][route] = std::vector<std::pair<double, double>> ();
            }
            map[type][route].push_back(coords);
        }
    }
    std::unordered_map<std::string, std::pair<std::string, double>> ans;
    for (const auto& [type, routes] : map) {
        auto [route, points] = *std::max_element(routes.begin(), routes.end(), [](const auto& p1, const auto& p2) {
            const std::vector<std::pair<double, double>>& points1 = p1.second;
            const std::vector<std::pair<double, double>>& points2 = p2.second;
            return mst_weight(points1) < mst_weight(points2);
        });
        ans[type] = std::make_pair(route, mst_weight(points));
    }
    return ans;
}

std::pair<std::string, int> task3(const pugi::xml_node& dataset) {
    static const std::vector<std::string> bad_words {
        "ул\\.",
        "пр\\.",
        "улица",
        "проспект",
        "проезд",
        "проспект",
        "переулок",
        "пер\\.",
        "мост",
        "аллея",
        "ш\\.",
        "шоссе",
        "набережная",
        "наб\\.",
        "реки",
        "р\\.",
        "пл\\.",
        "площадь",
        "бульвар",
        "бул\\.",
        "дорога",
        "дор\\.",
        "канала"
    };
    static const std::regex location_split_rgx ("\\s*,\\s*");
    static const std::regex word_split_rgx ("\\s+|"+join(bad_words,"|"));
//    static const std::regex word_split_rgx ("\\s+");
    static const std::regex two_lines_rgx("(\\d+)-(\\d+)-я линии(?: В\\.О\\.)?");
    std::unordered_map<std::string, int> map;
    for (pugi::xml_node station = dataset.first_child(); station; station = station.next_sibling()) {
        std::string location = station.child_value("location");
        std::vector<std::string> full_streets = split(location, location_split_rgx, false);
        for (const std::string& full_street : full_streets) {
            std::smatch smatch;
            std::string short_street;
            if (full_street.empty()) {
                continue;
            }
            else if (std::regex_match(full_street, smatch, two_lines_rgx)) {
                std::string first_line = smatch[1].str() + "-я линия";
                if (!map.count(first_line)) {
                    map[first_line] = 0;
                }
                map[first_line]++;
                short_street = smatch[2].str() + "-я линия";
            }
            else {
                std::vector<std::string> words = split(full_street, word_split_rgx, false);
//                if (words.size() == 1) {
//                    std::cout << full_street << std::endl;
//                }
                short_street = join(words, " ");
            }
            if (!map.count(short_street)) {
                map[short_street] = 0;
//                std::cout << "'" << full_street << "' : '" << short_street << "'" << std::endl;
            }
            map[short_street]++;
        }
    }
    return *std::max_element(map.begin(), map.end(), comp_by_value<std::string, int>);
}

std::string stations(int n) {
    switch (n % 10) {
        case 1:
            return "остановка";
        case 2:
        case 3:
        case 4:
            return "остановки";
        default:
            return "остановок";
    }
}

int main() {
    std::setlocale(LC_ALL, "");

    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file("data.xml");

    if (!res) {
        std::cout << "error loading file" << std::endl;
        exit(1);
    }

    pugi::xml_node dataset = doc.child("dataset");

    std::cout << "Task 1:" << std::endl;
    std::unordered_map<std::string, std::pair<std::string, int>> ans1 = task1(dataset);
    for (const auto&[type, champ] : ans1) {
        const auto&[route, n] = champ;
        std::cout << type << ": " << route << " (" << n << " " << stations(n) << ")" << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Task 2:" << std::endl;
    std::unordered_map<std::string, std::pair<std::string, double>> ans2 = task2(dataset);
    for (const auto& [type, champ] : ans2) {
        const auto& [route, len] = champ;
        std::cout << type << ": " << route << " (" << len << " м.)" << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Task 3" << std::endl;
    std::pair<std::string, int> ans3 = task3(dataset);
    const auto&[street, n] = ans3;
    std::cout << street << " (" << n << " " << stations(n) << ")" << std::endl;
}
