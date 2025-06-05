#include <json.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <string>

using json = nlohmann::json;

using std::vector;

using std::map;

using std::string;

string func1()
{
    json js;
    js["id_type"] = 2;
    js["msg"] = json::object();
    js["msg"]["lisi"] = "hello";
    js["msg"]["zhangsan"] = "what?";
    std::string s = js.dump();
    std::cout << s.c_str() << std::endl;
    return s;
}
string func2()
{
    json js;
    vector<int> res;
    res.push_back(1);
    res.push_back(2);
    res.push_back(5);

    js["list"] = res;

    map<int, std::string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "四姑娘山"});
    js["map"] = m;
    std::cout << js << std::endl;
    return js.dump();
}

int main()
{
    string s1 = func1();
    string s2 = func2();

    json s3 = json::parse(s1);
    json s4 = json::parse(s2);
    std::cout << s3["msg"] << "\n"
              << s4["ma"] << std::endl;

    vector<int> arr = s4["list"];
    // std::cout<<arr.size();
    for (int &v : arr)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    map<int, string> m = s4["map"];
    for (auto &it : m)
    {
        std::cout << it.first << " " << it.second << " ";
    }
    std::cout << std::endl;
}