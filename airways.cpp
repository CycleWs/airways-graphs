#include <bits/stdc++.h>
#include <fstream>


using namespace std;

// Estrutura para representar uma aresta
struct Edge {
    // origem, destino e peso
    int src, dest, weight;
};

map<string, int> states = {
        {"AC", 0},
        {"AL", 1},
        {"AM", 2},
        {"AP", 3},
        {"BA", 4},
        {"CE", 5},
        {"DF", 6},
        {"ES", 7},
        {"GO", 8},
        {"MA", 9},
        {"MG", 10},
        {"MS", 11},
        {"MT", 12},
        {"PA", 13},
        {"PB", 14},
        {"PE", 15},
        {"PI", 16},
        {"PR", 17},
        {"RJ", 18},
        {"RN", 19},
        {"RO", 20},
        {"RR", 21},
        {"RS", 22},
        {"SC", 23},
        {"SE", 24},
        {"SP", 25},
        {"TO", 26}
    };

// Classe para representar um grafo
class Graph {
    int V; // Número de vértices
    vector<vector<int>> adjacency_matrix; // Matriz de adjacência
public:
    vector<int> vertices_com_mau_tempo; // Vetor para acompanhar os vértices com mau tempo

    // Construtor
    Graph(int V) : V(V) {
        // Inicializa a matriz de adjacência com 0
        adjacency_matrix.resize(V, vector<int>(V, 0));
    }

    // Função para adicionar uma aresta ao grafo
    void addEdge(int src, int dest, int weight) {
        if (src != dest) // Evita adicionar uma aresta do vértice para ele mesmo
            adjacency_matrix[src][dest] = weight;
    }

    // Função para obter o vértice adjacente com o menor peso
    int getMinAdjacent(int vertex, vector<bool>& visited) {
        int min_weight = INT_MAX;
        int min_vertex = -1;

        for (int v = 0; v < V; ++v) {
            if (!visited[v] && adjacency_matrix[vertex][v] > 0 && adjacency_matrix[vertex][v] < min_weight) {
                min_weight = adjacency_matrix[vertex][v];
                min_vertex = v;
            }
        }

        return min_vertex;
    }

    // Função para encontrar o caminho mais rápido de A para B, evitando vértices com mau tempo
    vector<int> shortestPath(int src, int dest) {
        vector<bool> visited(V, false); // Vetor para acompanhar os vértices visitados
        vector<int> distances(V, INT_MAX); // Vetor para acompanhar as distâncias dos vértices do vértice de origem
        vector<int> parents(V, -1); // Vetor para acompanhar os pais dos vértices no caminho mais curto

        distances[src] = 0; // A distância do vértice de origem para ele mesmo é 0

        for (int i = 0; i < V - 1; ++i) {
            int u = -1;
            for (int v = 0; v < V; ++v) {
                if (!visited[v] && (u == -1 || distances[v] < distances[u])) {
                    u = v;
                }
            }

            visited[u] = true;

            for (int v = 0; v < V; ++v) {
                if (!visited[v] && adjacency_matrix[u][v] > 0 && distances[u] != INT_MAX &&
                    distances[u] + adjacency_matrix[u][v] < distances[v]) {
                    // Verifica se o próximo vértice tem mau tempo
                    bool skip_vertex_with_bad_weather = false;
                    if (find(vertices_com_mau_tempo.begin(), vertices_com_mau_tempo.end(), v) != vertices_com_mau_tempo.end()) {
                        // Se for o destino ou não houver outra opção, permitimos passar por uma cidade com mau tempo
                        if (v == dest || getMinAdjacent(v, visited) == -1) {
                            skip_vertex_with_bad_weather = true;
                        }
                    }

                    if (!skip_vertex_with_bad_weather || v == dest) {
                        distances[v] = distances[u] + adjacency_matrix[u][v];
                        parents[v] = u;
                    }
                }
            }
        }

        // Reconstruir o caminho mais curto da origem ate o destino
        vector<int> path;
        int current = dest;
        while (current != -1) {
            path.push_back(current);
            current = parents[current];
        }

        reverse(path.begin(), path.end());

        return path;
    }

    // Função para marcar um vértice como tendo mau tempo
    void marcarMauTempo(int vertex) {
        if (find(vertices_com_mau_tempo.begin(), vertices_com_mau_tempo.end(), vertex) == vertices_com_mau_tempo.end()) {
            vertices_com_mau_tempo.push_back(vertex);
        }
    }

    // Função para imprimir o grafo e os vértices com mau tempo
    void printGrafoEMauTempo() {
        cout << "Estados:" << endl;
        for(auto itr=states.begin(); itr != states.end(); ++itr){
            cout << itr ->first << " : " << itr->second << endl;
        }

        
        cout << "Vertices com mau tempo:";
        for (int vertex : vertices_com_mau_tempo) {
            cout << " " << vertex;
        }
        cout << endl;
    }

    // Função para verificar se o grafo é conexo
    bool isConexo() {
        vector<bool> visited(V, false); // Vetor para acompanhar os vértices visitados
        dfs(0, visited);

        // Verifica se todos os vértices foram visitados
        for (bool v : visited) {
            if (!v) {
                return false;
            }
        }
        return true;
    }

    void generateDotFile(const string& filename) {
        ofstream dotFile(filename); // Abrir o arquivo .dot

        // Escrever o cabeçalho do arquivo .dot
        dotFile << "digraph {" << endl;

        // Escrever as arestas do grafo no formato "src -> dest"
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (adjacency_matrix[i][j] > 0) {
                    dotFile << i << " -> " << j << " [label=" << adjacency_matrix[i][j] << "]" << endl;
                }
            }
        }

        // Escrever o final do arquivo .dot
        dotFile << "}" << endl;

        // Fechar o arquivo
        dotFile.close();

        cout << "Arquivo " << filename << " gerado com sucesso." << endl;
    }

    void generateDotFileWithPath(const Graph& graph, const vector<int>& shortestPath, const string& filename) {
        ofstream dotFile(filename); // Abrir o arquivo .dot

        // Escrever o cabeçalho do arquivo .dot
        dotFile << "digraph {" << endl;

        // Escrever as arestas do grafo no formato "src -> dest"
        for (int i = 0; i < graph.V; ++i) {
            for (int j = 0; j < graph.V; ++j) {
                if (graph.adjacency_matrix[i][j] > 0) {
                    // Verificar se a aresta pertence ao caminho mais curto
                    auto it = find(shortestPath.begin(), shortestPath.end(), i);
                    if (it != shortestPath.end() && next(it) != shortestPath.end() && *(next(it)) == j) {
                        dotFile << i << " -> " << j << " [label=" << graph.adjacency_matrix[i][j] << " color=red];" << endl;
                    } else {
                        dotFile << i << " -> " << j << " [label=" << graph.adjacency_matrix[i][j] << "];" << endl;
                    }
                }
            }
        }

        // Escrever o final do arquivo .dot
        dotFile << "}" << endl;

        // Fechar o arquivo
        dotFile.close();

        cout << "Arquivo " << filename << " gerado com sucesso." << endl;
    }

    // Função para garantir que cada vértice tenha pelo menos uma conexão de entrada e uma conexão de saída
    void garantirConexoesMinimas() {
        for (int i = 0; i < V; ++i) {
            bool has_outgoing_edge = false;
            bool has_incoming_edge = false;
            for (int j = 0; j < V; ++j) {
                if (adjacency_matrix[i][j] > 0) {
                    has_outgoing_edge = true;
                }
                if (adjacency_matrix[j][i] > 0) {
                    has_incoming_edge = true;
                }
            }
            if (!has_outgoing_edge) {
                int dest;
                do {
                    dest = rand() % V;
                } while (dest == i || adjacency_matrix[dest][i] > 0); // Garante que não seja o mesmo vértice ou já tenha uma conexão de entrada
                int weight = rand() % 10 + 1; // Peso aleatório entre 1 e 10
                adjacency_matrix[i][dest] = weight;
            }
            if (!has_incoming_edge) {
                int src;
                do {
                    src = rand() % V;
                } while (src == i || adjacency_matrix[i][src] > 0); // Garante que não seja o mesmo vértice ou já tenha uma conexão de saída
                int weight = rand() % 10 + 1; // Peso aleatório entre 1 e 10
                adjacency_matrix[src][i] = weight;
            }
        }
    }

private:
    // Função de busca em profundidade (DFS)
    void dfs(int v, vector<bool>& visited) {
        visited[v] = true;
        for (int i = 0; i < V; ++i) {
            if (adjacency_matrix[v][i] > 0 && !visited[i]) {
                dfs(i, visited);
            }
        }
    }
};



int main() {
    // Inicializando a semente para geração de números aleatórios
    srand(time(0));
    int quantity_vertex = 27;
    
    // Criando o grafo com 10 vértices
    Graph graph(quantity_vertex);

    // Adicionando pelo menos uma conexão de saída para cada vértice
    for (int i = 0; i < quantity_vertex; ++i) {
        int j;
        do {
            j = rand() % quantity_vertex; // Seleciona um vértice aleatório
        } while (j == i); // Garante que não seja o mesmo vértice
        int weight = rand() % 10 + 1; // Peso aleatório entre 1 e 10
        graph.addEdge(i, j, weight);
    }

    // Adicionando conexões aleatórias adicionais
    for (int i = 0; i < quantity_vertex; ++i) {
        int num_connections = rand() % 5; // Número aleatório de conexões adicionais (até 5)
        for (int c = 0; c < num_connections; ++c) {
            int j = rand() % 10;
            int weight = rand() % 10 + 1; // Peso aleatório entre 1 e 10
            graph.addEdge(i, j, weight);
        }
    }

    // Garantindo que cada vértice tenha pelo menos uma conexão de entrada e uma conexão de saída
    graph.garantirConexoesMinimas();

    // Marcando alguns vértices como tendo mau tempo (aleatoriamente)
    for (int i = 0; i < 9; ++i) {
        int vertex;
        do {
            vertex = rand() % 10;
        } while (find(graph.vertices_com_mau_tempo.begin(), graph.vertices_com_mau_tempo.end(), vertex) != graph.vertices_com_mau_tempo.end());
        graph.marcarMauTempo(vertex);
    }

    // Imprimindo o grafo e os vértices com mau tempo
    graph.printGrafoEMauTempo();

    graph.generateDotFile("grafo.dot");
    
    // Obtendo os pontos de origem e destino do usuário
    int source, destination;

    string estado_origem;
    string estado_destino;

    cout << "Digite o nome de um estado de origem: ";
    cin >> estado_origem;
    cout << "Digite o nome do estado do destino: ";
    cin >> estado_destino;

    source = states[estado_origem];
    destination = states[estado_destino];
    

    // Calculando o caminho mais rápido de A para B, evitando vértices com mau tempo
    vector<int> shortestPath = graph.shortestPath(source, destination);

    // Gerando o arquivo .dot para o caminho selecionado
    graph.generateDotFileWithPath(graph, shortestPath, "caminho.dot");

    // Exibindo o caminho mais rápido
    cout << "Caminho mais rápido de " << source << " para " << destination << ": ";
    for (int vertex : shortestPath) {
        cout << vertex << " ";
    }
    cout << endl;

    return 0;
}