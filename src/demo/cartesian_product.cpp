//
// Created by shulingjie on 2023/3/16.
//
#include "common_struct.h"
#include <numeric>

template<typename Element>
vector<vector<Element>> CartesianProduct(vector<vector<Element>> &tag_values_list) {
    vector<vector<Element>> res;
    size_t n = tag_values_list.size();
    vector<Element> single_res(n);
    for (int i = 0; i < n; ++i) {
        single_res[i] = tag_values_list[i][0];
    }
    res.emplace_back(single_res);

    for (int i = n - 1; i >= 0; --i) {
        int res_size = res.size();
        for (int k = 1; k < tag_values_list[i].size(); ++k) {
            for (int j = 0; j < res_size; ++j) {  // 将res里已有的结果全部取出，然后一一替换对应位置的元素，得到新的一种组合
                single_res = res[j];
                single_res[i] = tag_values_list[i][k];
                res.emplace_back(std::move(single_res)); // 这里会调用一次 single_res 的 copy constructor，从而插入一个全新的 vector
            }
        }
    }

    return res;
}

//template<typename Element>
using Element = string;
vector<vector<Element *>> CartesianProduct(vector<vector<Element>> &tag_values_list) {
    vector<vector<Element *>> all_product_result;
    size_t n = tag_values_list.size();
    vector<Element *> one_product_result(n);
    for (int i = 0; i < n; ++i) {
        one_product_result[i] = &(tag_values_list[i][0]);
    }
    all_product_result.emplace_back(std::move(one_product_result));

    for (int i = n - 1; i >= 0; --i) {
        int res_size = all_product_result.size();
        for (int k = 1; k < tag_values_list[i].size(); ++k) {
            for (int j = 0; j < res_size; ++j) {  // 将res里已有的结果全部取出，然后一一替换对应位置的元素，得到新的一种组合
                auto old_single_res = all_product_result[j];
                vector<Element *> new_single_res(old_single_res);
                new_single_res[i] = &(tag_values_list[i][k]);
                all_product_result
                    .emplace_back(std::move(new_single_res)); // 这里会调用一次 single_res 的 copy constructor，从而插入一个全新的 vector
            }
        }
    }

    return all_product_result;
}

vector<vector<string>> CartesianProduct2(vector<vector<string>> &v) {
    auto product = [](long long a, const vector<string> &b) { return a * b.size(); };
    const long long product_n = std::accumulate(v.begin(), v.end(), 1LL, product);

    vector<vector<string>> all_product_result;
    for (int n = 0; n < product_n; n++) {
        vector<string> one_product_result(v.size());

        lldiv_t q{n, 0};
        for (int i = v.size() - 1; i >= 0; i--) {
            cout << "quot=" << q.quot << ", rem=" << q.rem << endl;
            q = div(q.quot, v[i].size());
            one_product_result[i] = v[i][q.rem];
        }
        all_product_result.emplace_back(std::move(one_product_result));
    }
    return all_product_result;
}

vector<vector<const string *>> CartesianProduct3(const vector<vector<string>> &v) {
    auto product = [](long long a, const vector<string> &b) { return a * b.size(); };
    const long long product_n = std::accumulate(v.begin(), v.end(), 1LL, product);

    vector<vector<const string *>> ret;
    for (int n = 0; n < product_n; n++) {
        vector<const string *> one_product_result(v.size());

        lldiv_t q{n, 0};
        for (int i = v.size() - 1; i >= 0; i--) {
            q = div(q.quot, v[i].size());
            one_product_result[i] = &(v[i][q.rem]);
        }
        ret.emplace_back(std::move(one_product_result));
    }
    return ret;
}

vector<vector<string> > expect{
    {"1", "4", "6",},
    {"1", "4", "7",},
    {"1", "4", "8",},
    {"1", "5", "6",},
    {"1", "5", "7",},
    {"1", "5", "8",},
    {"2", "4", "6",},
    {"2", "4", "7",},
    {"2", "4", "8",},
    {"2", "5", "6",},
    {"2", "5", "7",},
    {"2", "5", "8",},
    {"3", "4", "6",},
    {"3", "4", "7",},
    {"3", "4", "8",},
    {"3", "5", "6",},
    {"3", "5", "7",},
    {"3", "5", "8",},
    {"1", "4", "6",},
    {"1", "4", "7",},
    {"1", "4", "8",},
    {"1", "5", "6",},
    {"1", "5", "7",},
    {"1", "5", "8",},
    {"2", "4", "6",},
    {"2", "4", "7",},
    {"2", "4", "8",},
    {"2", "5", "6",},
    {"2", "5", "7",},
    {"2", "5", "8",},
    {"3", "4", "6",},
    {"3", "4", "7",},
    {"3", "4", "8",},
    {"3", "5", "6",},
    {"3", "5", "7",},
    {"3", "5", "8",},
};

vector<vector<string> > v{{"1", "2", "3"},
                          {"4", "5"},
                          {"6", "7", "8"}};

void Print() {

    for (int i = 0; i < 2; i++) {
        auto ans = CartesianProduct2(v);
        assert(std::equal(ans.begin(), ans.end(), expect.begin()));
        int size = 0;
//        for (const auto &first: ans) {
//            size++;
//            cout << size << ": ";
//            for (const auto &second: first) {
//                cout << second << ",";
//            }
//            cout << endl;
//        }

        auto ans3 = CartesianProduct3(v);
        size = 0;
        for (const auto &first: ans3) {
            size++;
            cout << size << ": ";
            for (const auto &second: first) {
                cout << *second << ", ";
            }
            cout << endl;
        }

        auto ans2 = CartesianProduct(v);
//        assert(std::equal(ans2.begin(), ans2.end(), ans3.begin()));
        size = 0;
        for (const auto &first: ans2) {
            size++;
            cout << size << ": ";
            for (const auto &second: first) {
                cout << *second << ", ";
            }
            cout << endl;
        }
    }
}

void PerfTest() {
    int round = 10;
    int loop = 100000;

    auto t1 = chrono::steady_clock::now();
    for (int r = 0; r < round; r++) {
        for (int i = 0; i < loop; i++) {
            auto ans = CartesianProduct(v);
        }
    }
    auto t2 = chrono::steady_clock::now();
    cout << (chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()) / round << endl;

    t1 = chrono::steady_clock::now();
    for (int r = 0; r < round; r++) {
        for (int i = 0; i < loop; i++) {
            auto ans = CartesianProduct2(v);
        }
    }
    t2 = chrono::steady_clock::now();
    cout << (chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()) / round << endl;

    t1 = chrono::steady_clock::now();
    for (int r = 0; r < round; r++) {
        for (int i = 0; i < loop; i++) {
            auto ans = CartesianProduct3(v);
        }
    }
    t2 = chrono::steady_clock::now();
    cout << (chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()) / round << endl;
}

int main(int argc, char **argv) {
//    for (int i = 0; i < 18; i++) {
//        ::lldiv_t  q{18, 0};
//        q = div(q.quot, 3LL);
//        cout<<q.quot << ", " << q.rem << endl;
//        q = div(q.quot, 2LL);
//        cout<<q.quot << ", " << q.rem << endl;
//        q = div(q.quot, 3LL);
//        cout<<q.quot << ", " << q.rem << endl;
//    }

//    Print();
//    PerfTest();

    vector<vector<string> > v1{{""},
                               {"4", "5"},
                               {"6", "7", "8"}};
    auto res = CartesianProduct3(v1);
    int size = 0;
    for (const auto &first: res) {
        size++;
        cout << size << ": ";
        for (const auto &second: first) {
            cout << *second << ", ";
        }
        cout << endl;
    }

}