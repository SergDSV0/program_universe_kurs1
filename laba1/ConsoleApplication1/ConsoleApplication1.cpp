#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Классы для характеристик устройств (GpuSpec, CpuSpec, RamSpec, LanSpec) и узлов кластера (ClusterNode) остаются неизменными.
// Оставлю их без повторного описания, чтобы код был компактнее.

class GpuSpec {
public:
    string model;
    int cores;
    int memory; // В МБ

    void Print() const {
        cout << "GPU: Model: " << model << ", Cores: " << cores << ", Memory: " << memory << " MB" << endl;
    }

    void Import(ifstream& file) {
        file >> model >> cores >> memory;
    }

    void Export(ofstream& file) const {
        file << model << " " << cores << " " << memory << endl;
    }
};

class CpuSpec {
public:
    string model;
    int cores;
    double frequency; // В ГГц

    void Print() const {
        cout << "CPU: Model: " << model << ", Cores: " << cores << ", Frequency: " << frequency << " GHz" << endl;
    }

    void Import(ifstream& file) {
        file >> model >> cores >> frequency;
    }

    void Export(ofstream& file) const {
        file << model << " " << cores << " " << frequency << endl;
    }
};

class RamSpec {
public:
    int capacity; // В МБ
    string type;

    void Print() const {
        cout << "RAM: Capacity: " << capacity << " MB, Type: " << type << endl;
    }

    void Import(ifstream& file) {
        file >> capacity >> type;
    }

    void Export(ofstream& file) const {
        file << capacity << " " << type << endl;
    }
};

class LanSpec {
public:
    double bandwidth; // В Гбит/с
    string type;

    void Print() const {
        cout << "LAN: Bandwidth: " << bandwidth << " Gbps, Type: " << type << endl;
    }

    void Import(ifstream& file) {
        file >> bandwidth >> type;
    }

    void Export(ofstream& file) const {
        file << bandwidth << " " << type << endl;
    }
};

class ClusterNode {
public:
    GpuSpec gpu;
    CpuSpec cpu;
    RamSpec ram;
    LanSpec lan;

    void Print() const {
        cout << "Cluster Node:" << endl;
        gpu.Print();
        cpu.Print();
        ram.Print();
        lan.Print();
    }

    void Import(ifstream& file) {
        gpu.Import(file);
        cpu.Import(file);
        ram.Import(file);
        lan.Import(file);
    }

    void Export(ofstream& file) const {
        gpu.Export(file);
        cpu.Export(file);
        ram.Export(file);
        lan.Export(file);
    }
};

class Cluster {
public:
    vector<ClusterNode> nodes;

    void AddNode(const ClusterNode& node) {
        nodes.push_back(node);
    }

    void Print() const {
        cout << "Cluster:" << endl;
        for (size_t i = 0; i < nodes.size(); ++i) {
            cout << "Node " << i + 1 << ":" << endl;
            nodes[i].Print();
        }
    }

    void Import(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        size_t nodeCount;
        file >> nodeCount;
        nodes.clear();

        for (size_t i = 0; i < nodeCount; ++i) {
            ClusterNode node;
            node.Import(file);
            nodes.push_back(node);
        }
    }

    void Export(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        file << nodes.size() << endl;
        for (const auto& node : nodes) {
            node.Export(file);
        }
    }
};


int main() {
    // первый узел кластера
    ClusterNode node1;
    node1.gpu = { "NVIDIA RTX 3090", 10496, 24576 };
    node1.cpu = { "AMD Ryzen 9 5900X", 12, 3.7 };
    node1.ram = { 32768, "DDR4" };
    node1.lan = { 10.0, "Ethernet" };

    // второй узел кластера
    ClusterNode node2;
    node2.gpu = { "NVIDIA RTX 4060", 3072, 8192 };
    node2.cpu = { "Intel Core i5-12400F", 6, 2.5 };
    node2.ram = { 16384, "DDR4" };
    node2.lan = { 2.5, "Ethernet" };
    //  кластер
    Cluster cluster;
    cluster.AddNode(node1);
    cluster.AddNode(node2);


    cluster.Print();

    cluster.Export("cluster_data.txt");

   
    Cluster importedCluster;
    importedCluster.Import("cluster_data.txt");
    importedCluster.Print();

    return 0;
}