#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <chrono>

using namespace std;

vector<vector<string>> datas;
vector<bool> is_continuous_features;


struct Node {
    bool is_leaf;
    string class_label; 
    int feature_index;  
    double threshold;        
    string category_value;   
    Node* left;
    Node* right;
    Node()
      : is_leaf(false),
        feature_index(-1),
        threshold(0.0),
        left(nullptr),
        right(nullptr)
    {}
    ~Node() {
        delete left;
        delete right;
    }
};

// // Function to compute most frequent value for categorical columns
// string get_most_frequent_value(const vector<vector<string>>& data, int feature_index) {
//     map<string, int> value_counts;
//     for (const auto& row : data) {
//         if (row[feature_index] != "?") {
//             value_counts[row[feature_index]]++;
//         }
//     }
//     string most_frequent;
//     int max_count = 0;
//     for (const auto& [value, count] : value_counts) {
//         if (count > max_count) {
//             max_count = count;
//             most_frequent = value;
//         }
//     }
//     return most_frequent;
// }

// // Function to compute mean for continuous columns
// double get_mean_value(const vector<vector<string>>& data, int feature_index) {
//     double sum = 0.0;
//     int count = 0;
//     for (const auto& row : data) {
//         if (row[feature_index] != "?") {
//             try {
//                 sum += stod(row[feature_index]);
//                 count++;
//             } catch (...) {
//                 // Skip invalid numeric values
//             }
//         }
//     }
//     return count > 0 ? sum / count : 0.0;
// }

// // Function to replace missing values
// vector<vector<string>> replace_missing_values(vector<vector<string>>& data, const vector<bool>& is_continuous) {
//     vector<vector<string>> processed_data = data;
//     for (size_t feature = 0; feature < is_continuous.size(); ++feature) {
//         if (is_continuous[feature]) {
//             double mean = get_mean_value(data, feature);
//             string mean_str = to_string(mean);
//             for (auto& row : processed_data) {
//                 if (row[feature] == "?") {
//                     row[feature] = mean_str;
//                 }
//             }
//         } else {
//             string most_frequent = get_most_frequent_value(data, feature);
//             for (auto& row : processed_data) {
//                 if (row[feature] == "?") {
//                     row[feature] = most_frequent;
//                 }
//             }
//         }
//     }
//     return processed_data;
// }

// vector<vector<string>> load_adult_full(const string& filename) {
//     ifstream file(filename);
//     if (!file.is_open()) {
//         cerr << "Error opening Adult dataset file: " << filename << endl;
//         exit(1);
//     }
//     vector<vector<string>> adult_datas;
//     string line;
//     while (getline(file, line)) {
//         stringstream ss(line);
//         string token;
//         vector<string> row;
//         while (getline(ss, token, ',')) {
//             // Remove leading/trailing whitespace
//             token.erase(0, token.find_first_not_of(" \t"));
//             token.erase(token.find_last_not_of(" \t") + 1);
//             row.push_back(token);
//         }
//         if (row.size() == 15) {
//             adult_datas.push_back(row);
//         }
//     }
//     file.close();
//     // Replace missing values
//     return replace_missing_values(adult_datas, is_continuous_features);
// }

// vector<vector<string>> load_adult_sampled(const string& filename, int sample_size = 1000) {
//     vector<vector<string>> all_data = load_adult_full(filename);
    
//     // Randomly sample rows
//     vector<vector<string>> adult_datas;
//     if (all_data.empty()) return adult_datas;
//     random_device rd;
//     mt19937 rng(rd());
//     vector<int> indices(all_data.size());
//     iota(indices.begin(), indices.end(), 0);
//     shuffle(indices.begin(), indices.end(), rng);
//     int n = min(sample_size, (int)all_data.size());
//     for (int i = 0; i < n; i++) {
//         adult_datas.push_back(all_data[indices[i]]);
//     }
//     return adult_datas;
// }


vector<vector<string>> load_iris(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        exit(1);
    }
    vector<vector<string>> iris_data;
    string line;
    getline(file, line);  // skip header
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() == 6) {
            vector<string> row(tokens.begin() + 1, tokens.begin() + 5);
            row.push_back(tokens[5]);
            iris_data.push_back(row);
        }
    }
    file.close();
    return iris_data;
}

vector<vector<string>> load_adult_full(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening Adult datasset file: " << filename << endl;
        exit(1);
    }
    vector<vector<string>> adult_datas;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> row;
        bool has_missing = false;
        while (ss >> token) {
            if (token == "?") 
                has_missing = true;
            row.push_back(token);
        }
        if (!has_missing && row.size() == 15) {
            adult_datas.push_back(row);
        }
    }
    file.close();
    return adult_datas;
}

vector<vector<string>> load_adult_sampled(const string& filename, int sample_size = 1000) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening Adult datasset file: " << filename << endl;
        exit(1);
    }
    vector<vector<string>> valid_rows;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> row;
        bool has_missing = false;
        while (ss >> token) {
            if (token == "?") has_missing = true;
            row.push_back(token);
        }
        if (!has_missing && row.size() == 15) { 
            valid_rows.push_back(row);
        }
    }
    file.close();

    //randomly sample rows of sample size
    vector<vector<string>> adult_datas;
    if (valid_rows.empty()) return adult_datas;
    random_device rd;
    mt19937 rng(rd());
    vector<int> indices(valid_rows.size());
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), rng);
    int n = min(sample_size, (int)valid_rows.size());
    for (int i = 0; i < n; i++) {
        adult_datas.push_back(valid_rows[indices[i]]);
    }
    return adult_datas;
}

double compute_entropy(const vector<int>& indices) {
    if (indices.empty()) return 0.0;
    map<string,int> label_counts;
    for (int i : indices) {
        label_counts[datas[i].back()]++;
    }
    double total = indices.size();
    double entropy = 0.0;
    for (auto& [label, count] : label_counts) {
        double p = count / total;
        if (p > 0) {
            entropy -= p * log2(p);
        }
    }
    return entropy;
}

pair<vector<int>,vector<int>>split_continuous(const vector<int>& indices, int featureIndex, double threshold) {
    vector<int> left_indices, right_indices;
    for (int i : indices) {
        double value = stod(datas[i][featureIndex]);
        if (value <= threshold)
            left_indices.push_back(i);
        else
            right_indices.push_back(i);
    }
    return {left_indices, right_indices};
}

pair<vector<int>,vector<int>>split_categorical(const vector<int>& indices, int featureIndex, const string& category) {
    vector<int> matching, nonmatching;
    for (int i : indices) {
        if (datas[i][featureIndex] == category)
            matching.push_back(i);
        else
            nonmatching.push_back(i);
    }
    return {matching, nonmatching};
}

int count_unique_values(const vector<int>& indices, int featureIndex) {
    set<string> unique_vals;
    for (int i : indices) {
        unique_vals.insert(datas[i][featureIndex]);
    }
    return unique_vals.size();
}


string get_majority_label(const vector<int>& indices) {
    if (indices.empty()) return "";
    map<string, int> counts;
    for (int i : indices) counts[datas[i].back()]++;
    string label;
    int max_count = 0;
    for (const auto& [lbl, cnt] : counts) {
        if (cnt > max_count) {
            max_count = cnt;
            label = lbl;
        }
    }
    return label;
}

bool all_labels_identical(const vector<int>& indices) {
    if (indices.empty()) return true;
    const string& first_label = datas[indices[0]].back();
    for (int i : indices) {
        if (datas[i].back() != first_label) return false;
    }
    return true;
}

double compute_split_info(double p_left, double p_right) {
    double iv = 0.0;
    if (p_left > 0)  iv -= p_left * log2(p_left);
    if (p_right > 0) iv -= p_right * log2(p_right);
    return iv;
}

pair<double,double>find_best_continuous_split(const vector<int>& indices,int featureIndex,const string& criterion) {
    vector<double> values;
    for (int i : indices) {
        values.push_back(stod(datas[i][featureIndex]));
    }
    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
    if (values.size() < 2) {
        return {0.0, 0.0};
    }
    double parent_entropy = compute_entropy(indices);
    double best_score = -1.0;
    double best_threshold = values[0];

    for (size_t j = 0; j + 1 < values.size(); j++) {
        double threshold = (values[j] + values[j+1]) / 2.0;
        auto [left_inds, right_inds] = split_continuous(indices, featureIndex, threshold);
        double p_left  = double(left_inds.size())  / indices.size();
        double p_right = double(right_inds.size()) / indices.size();
        double ig = parent_entropy - p_left  * compute_entropy(left_inds)- p_right * compute_entropy(right_inds);

        double score = 0.0;
        if (criterion == "IG") {
            score = ig;
        }
        else if (criterion == "IGR") {
            double iv = compute_split_info(p_left, p_right);
            score = (iv > 0) ? ig / iv : 0.0;
        }
        else { 
            int unique_count = count_unique_values(indices, featureIndex);
            score = (ig / log2(unique_count + 1)) * (1.0 - double(unique_count - 1) / indices.size());
        }

        if (score > best_score) {
            best_score     = score;
            best_threshold = threshold;
        }
    }

    return {best_score, best_threshold};
}

pair<double,string>find_best_categorical_split(const vector<int>& indices,int featureIndex,const string& criterion){
    set<string> unique_vals;
    for (int i : indices) {
        unique_vals.insert(datas[i][featureIndex]);
    }
    double parent_entropy = compute_entropy(indices);
    double best_score = -1.0;
    string best_category = "";

    for (const string& category : unique_vals) {
        auto [matching_inds, nonmatching_inds] = split_categorical(indices, featureIndex, category);

        double p_match   = double(matching_inds.size())   / indices.size();
        double p_nonmatch = double(nonmatching_inds.size()) / indices.size();
        double ig = parent_entropy - p_match    * compute_entropy(matching_inds) - p_nonmatch * compute_entropy(nonmatching_inds);

        double score = 0.0;
        if (criterion == "IG") {
            score = ig;
        }
        else if (criterion == "IGR") {
            double iv = compute_split_info(p_match, p_nonmatch);
            score = (iv > 0) ? ig / iv : 0.0;
        }
        else { 
            int unique_count = count_unique_values(indices, featureIndex);
            score = (ig / log2(unique_count + 1)) * (1.0 - double(unique_count - 1) / indices.size());
        }

        if (score > best_score) {
            best_score     = score;
            best_category  = category;
        }
    }

    return {best_score, best_category};
}

Node* build_tree(const vector<int>& indices,int depth,int max_depth,const string& criterion) {
    Node* node = new Node();

    if ((max_depth > 0 && depth >= max_depth) || all_labels_identical(indices))
    {
        node->is_leaf= true;
        node->class_label = get_majority_label(indices);
        return node;
    }

    bool has_split = false;
    for (int f = 0; f < (int)datas[0].size() - 1; f++) {
        if (count_unique_values(indices, f) > 1) {
            has_split = true;
            break;
        }
    }
    if (!has_split) {
        node->is_leaf     = true;
        node->class_label = get_majority_label(indices);
        return node;
    }

    double best_split_score = -1.0;
    int best_feature        = -1;
    double best_threshold   = 0.0;
    string best_category;

    for (int feature = 0; feature < (int)datas[0].size() - 1; feature++) {
        if (is_continuous_features[feature]) {
            auto [score, threshold]= find_best_continuous_split(indices, feature, criterion);
            if (score > best_split_score) {
                best_split_score = score;
                best_feature     = feature;
                best_threshold   = threshold;
            }
        }
        else {
            auto [score, category]= find_best_categorical_split(indices, feature, criterion);
            if (score > best_split_score) {
                best_split_score = score;
                best_feature = feature;
                best_category= category;
            }
        }
    }

    node->feature_index = best_feature;
    if (is_continuous_features[best_feature]) {
        node->threshold = best_threshold;
        auto [left_inds, right_inds] = split_continuous(indices, best_feature, best_threshold);
        node->left  = build_tree(left_inds,depth + 1, max_depth, criterion);
        node->right = build_tree(right_inds,depth + 1, max_depth, criterion);
    }
    else {
        node->category_value = best_category;
        auto [match_inds, nonmatch_inds]= split_categorical(indices, best_feature, best_category);
        node->left  = build_tree(match_inds,depth + 1, max_depth, criterion);
        node->right = build_tree(nonmatch_inds, depth + 1, max_depth, criterion);
    }

    return node;
}

string predict(Node* node, const vector<string>& instance) {
    if (node->is_leaf) {
        return node->class_label;
    }
    int f = node->feature_index;
    if (is_continuous_features[f]) {
        double value = stod(instance[f]);
        if (value <= node->threshold)
            return predict(node->left, instance);
        else
            return predict(node->right, instance);
    }
    else {
        if (instance[f] == node->category_value)
            return predict(node->left, instance);
        else
            return predict(node->right, instance);
    }
}

int count_nodes(Node* t) {
    if (!t) 
        return 0;
    return 1 + count_nodes(t->left) + count_nodes(t->right);
}
int compute_tree_depth(Node* t) {
    if (!t) return 0;
    return 1+max(compute_tree_depth(t->left),compute_tree_depth(t->right));
}

void perform_experiments(const string& dataset_name) {
    vector<string> criteria = {"IG", "IGR", "NWIG"};
    vector<int> max_depths  = {2, 3, 5}; 
    int num_runs = 20;

    cout << "\n=== Results for " << dataset_name << " ===\n";
    for (const string& crit : criteria) {
        cout << "\n--- Criterion: " << crit << " ---\n";
        cout << "MaxDepth\tAvgAcc\tAvgNodes\tAvgDepth\n";
        for (int md : max_depths) {
            vector<double> accuracies;
            vector<int> node_counts, tree_depths;

            for (int run = 0; run < num_runs; run++) {
                mt19937 rng(run);
                vector<int> indices(datas.size());
                iota(indices.begin(), indices.end(), 0);
                shuffle(indices.begin(), indices.end(), rng);

                int split_point = indices.size() * 8 / 10;
                vector<int> train_indices(
                    indices.begin(), indices.begin() + split_point
                );
                vector<int> test_indices(
                    indices.begin() + split_point, indices.end()
                );

                Node* root = build_tree(train_indices, 0, md, crit);

                int correct = 0;
                for (int idx : test_indices) {
                    if (predict(root, datas[idx]) == datas[idx].back())
                        correct++;
                }
                accuracies.push_back(double(correct) / test_indices.size());
                node_counts .push_back(count_nodes(root));
                tree_depths.push_back(compute_tree_depth(root));

                delete root;
            }

            double avg_acc   = accumulate(accuracies.begin(),   accuracies.end(),   0.0) / num_runs;
            double avg_nodes = accumulate(node_counts.begin(),  node_counts.end(),  0.0) / num_runs;
            double avg_depth = accumulate(tree_depths.begin(),  tree_depths.end(),  0.0) / num_runs;

            cout << md<< "\t\t" << avg_acc<< "\t"   << avg_nodes<< "\t" << avg_depth-1<<endl;
        }
    }
}

void run_single_experiment(const string& criterion, int max_depth) {
    const int num_runs = 20;
    vector<int> all_indices(datas.size());
    iota(all_indices.begin(), all_indices.end(), 0);

    double sum_accuracy = 0;
    double sum_nodes    = 0;
    double sum_depth    = 0;

    for (int run = 0; run < num_runs; ++run) {
        mt19937 rng(run);
        vector<int> indices = all_indices;
        shuffle(indices.begin(), indices.end(), rng);

        int train_size = indices.size() * 8 / 10;
        vector<int> train_indices(indices.begin(), indices.begin() + train_size);
        vector<int> test_indices(indices.begin() + train_size, indices.end());

        Node* root = build_tree(train_indices, 0, max_depth, criterion);

        int correct = 0;
        for (int idx : test_indices) {
            if (predict(root, datas[idx]) == datas[idx].back()) {
                ++correct;
            }
        }
        double accuracy = double(correct) / test_indices.size();

        sum_accuracy += accuracy;
        sum_nodes    += count_nodes(root);
        sum_depth    += compute_tree_depth(root);

        delete root;
    }

    double avg_accuracy = sum_accuracy / num_runs;
    double avg_nodes    = sum_nodes    / num_runs;
    double avg_depth    = sum_depth    / num_runs;

    cout << "\nSingle experiment summary\n";
    cout << "Criterion: " << criterion<< "    MaxDepth: " << max_depth << endl<<endl;
    cout << "AvgAccuracy\tAvgNodes\tAvgDepth"<<endl;
    cout << avg_accuracy << "\t\t"<< avg_nodes   << "\t\t"<< avg_depth-1   << endl;
}

int main(int argc, char* argv[]) {
    
    auto start = chrono::high_resolution_clock::now();

    // datas = load_iris("Iris.csv");
    // is_continuous_features = vector<bool>(4, true);

    //Adult datasset (sampled)
    // datas = load_adult_sampled("adult.data", 2000);
    // is_continuous_features = {true, false, true, false, true, false, false, false, false, false, true, true, true, false};
    
    // Adult datasset (full)
    datas = load_adult_full("adult.data");
    is_continuous_features = {true, false, true, false, true, false, false, false, false, false, true, true, true, false};
    

    if (argc == 1) {
        //perform_experiments("Iris");
        perform_experiments("Adult (Sampled)");
         perform_experiments("Adult (Full)");
    }
    else if (argc == 3) {
        string criterion = argv[1];       
        int max_depth   = stoi(argv[2]);  
        run_single_experiment(criterion, max_depth);
    }
    else {
        cerr << "Usage:\n"
             << "  ./solve               # full experiments\n"
             << "  ./solve <CRIT> <D>    # single experiment\n"
             << "    <CRIT> = IG | IGR | NWIG\n"
             << "    <D>    = max tree depth\n";
        return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "\nTotal execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}