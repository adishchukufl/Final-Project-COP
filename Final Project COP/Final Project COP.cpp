// Final Project COP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <unordered_map>


using namespace std;


class WeightUndirectedGraph { 
    vector<pair<int, int>> *adjListGraph; //the first value will be the vertex name, and THE second will be the weight
    int vertices;
    int numOfRoads = 0; 

public:
    WeightUndirectedGraph(int numOfVertices) { //constructor for graph
        vertices = numOfVertices;
        adjListGraph = new vector<pair<int, int>>[vertices];
        
    }

    int getNumOfEdges() {
        return numOfRoads;
    }

    int getNumOfVerticies() {
        return vertices;
    }

    void addEdge(int vertexA, int vertexB, int dWeight) { //have to add edges to both placces, as the graph is undirected
        if (vertexA == vertexB) //incase we generate the same numbers
            return;
        adjListGraph[vertexA].push_back(make_pair(vertexB, dWeight));
        adjListGraph[vertexB].push_back(make_pair(vertexA, dWeight));
        numOfRoads++;
    }

    void deleteBigEdge(int vertexA) { //helper function createdd for the smaller graph specifically
        unordered_map <int, pair<int, int>> duplicates; //first is the index of the adjlist graph and second is the value of the weight
        auto aList = adjListGraph[vertexA]; //vector of pairs
        
        vector <int> indexsDeleted;
        
        for (int i = 0; aList.size() > i; i++) {
            if (duplicates.find(aList[i].first) == duplicates.end()) { //if we found no copies
                duplicates[aList[i].first] = make_pair(i, aList[i].second);
            }
            else {
                if (duplicates[aList[i].first].second < aList[i].second) { //if the weight in our umap is less than the one the list, we delete i
                    //if we find a lower weight we replace it 
                    indexsDeleted.push_back(i);
                    
                }
                else {
                    int index = duplicates[aList[i].first].first;
                    if (i < aList.size()) {
                        duplicates[aList[i].first] = make_pair(i, aList[i].second);
                    }
                    indexsDeleted.push_back(index);
                }

            }
            
        }
        int count = 0;
        for (int i = 0; i < indexsDeleted.size(); i++) {
            aList.erase(aList.begin() + (indexsDeleted[i] - count));
            count++;
        }
        adjListGraph[vertexA] = aList;

    }

    bool checkEdge(int vertexA, int vertexB) { // we will be checking if an edge already exists
        bool tick = false;

        for (int i = 0; adjListGraph[vertexA].size() > i; i++) {
            if (adjListGraph[vertexA][i].first != NULL && adjListGraph[vertexA][i].first == vertexB) {
                tick = true;
                break;
           }
        }
        return tick;
    }

    void generateGraph() {  //this is our function to create random number of graphs
         //will COUNT the number of roads we have currentl
        for (int i = 0; i < vertices; i++) {
            while (adjListGraph[i].size() < 4) { // we are filling up the graph with 4 edges per vertex
                if (adjListGraph[i].size() > 2 && i > (.7 * vertices)) { //once it fills up twice we are not going to fill it anymore, after 70%
                    break;
                }
                int randomNumberHelper = vertices - i;  
                int randomVertex = rand() % randomNumberHelper + i;
                //we are going to look for edges with higher vertex number to make it quicker to find random
                if (i > (.7 * vertices)) {
                    randomNumberHelper = vertices - .7 * vertices;
                    //WE want the the last 30 precent of the problem to be the stuff randomly generated
                    randomVertex = rand() % randomNumberHelper + (.7 * vertices - 1);
                }
               
                int randomWeight = rand() % 100 + 1; // km between roads 
                if (checkEdge(i, randomVertex) && checkEdge(randomVertex, i)) {
                    continue;
                }
                else {
                    if (adjListGraph[randomVertex].size() >= 4) { // we check if the other vertex has 4 edges already or not
                        continue;
                    } //if it doesn't then we add it
                    addEdge(i, randomVertex, randomWeight);
                }
            }
        }
        
    }

    vector<int> bellmanFord(int src) {
        vector<int> distance;

        for (int i = 0; i < vertices; i++) {
            distance.push_back(INT_MAX);
        }
        distance[src] = 0;

        //Number of Relaxations is equal to number of vertices
        for (int x = 1; x <= vertices - 1; x++) {
            //Double for loop iterates through AdjList, every single edge in the graph
            for (int y = 0; y < vertices; y++) {
                for (int z = 0; z < adjListGraph[y].size(); z++) {
                    int secVertex = adjListGraph[y].at(z).first;
                    int weight = adjListGraph[y].at(z).second;
                    //Relaxation
                    if (distance[y] != INT_MAX && distance[y] + weight < distance[secVertex])
                        distance[secVertex] = distance[y] + weight;
                }
            }
        }
        return distance;
    }

    vector<int> dijkstra(int src) {
        vector<int> distance(vertices, INT_MAX); //sets to infinity
        priority_queue<pair<int, int>> pQueue;  //creates priority queue
        pQueue.push({ distance[src] = 0, src }); //sets distance from itself to zero

        while (!pQueue.empty()) {
            int dist = pQueue.top().first;
            int vertex = pQueue.top().second;
            pQueue.pop();

            if (dist > distance[vertex]) {
                continue;
            }

            for (auto endVertex : adjListGraph[vertex]) {
                if (distance[vertex] + endVertex.second < distance[endVertex.first]) {
                    distance[endVertex.first] = distance[vertex] + endVertex.second;
                    pQueue.push({ distance[endVertex.first], endVertex.first });
                }
            }
        }
        return distance;
    }


    void printGraph() {
        for (int i = 0; vertices > i; i++) {
            cout << "Vertex " << i << " : ";
            for (auto itr : adjListGraph[i]) {
                cout << itr.first << " - " << itr.second << ", " ; 
            }
            cout << endl;
        }
    }

    int getTimeEdge(int vertexA, int vertexB) { //this function will find the weight of the edge
        for (int i = 0; adjListGraph[vertexA].size() > i; i++) {
            if (adjListGraph[vertexA][i].first == vertexB) {
                return adjListGraph[vertexA][i].second;
            }
        }
        
    }

    int shortestTimeToComplete(int numOfOrders) { //this function will be used on our smaller graph
        int totalTime = 0;
        bool* delievered = new bool[vertices];
        for (int i = 0; i < vertices; i++) {
            delievered[i] = false;
        }
        delievered[0] = true; //setting the first vertex true as that is our head quarters
        int currentVertex = 0; //STARTING Point
        int lowestVertex = 0;
        bool start = true;
        bool tick = true;
        while (start || !tick) {
            int lowestTime = { INT_MAX };
            for (int i = 1; i < vertices; i++) {
                if (getTimeEdge(currentVertex, i) < lowestTime && !delievered[i]) { //we look through all the vertices and see if they ahave been delievered
                    lowestTime = getTimeEdge(currentVertex, i);
                    lowestVertex = i;
                }
            }
            currentVertex = lowestVertex;
            totalTime += lowestTime;
            delievered[currentVertex] = true;
            start = false;
            tick = true;

            for (int i = 0; i < vertices; i++) {
                if (!delievered[i])
                    tick = false;
            }

        }
        return totalTime;
    }
};

WeightUndirectedGraph smallerGraph(vector<int> deliveryLocations, WeightUndirectedGraph& g, bool bellmans) { //where 
    WeightUndirectedGraph newGraph(deliveryLocations.size()+1);
    for (int i = 0; i < deliveryLocations.size(); i++) {
        if (bellmans) { //we do bellmans to create our graphs
            vector<int> results = g.bellmanFord(deliveryLocations[i]); 
            for (int j = 1; j < (deliveryLocations.size() + 1); j++) {
                newGraph.addEdge(i, j, results[j]);
            }
        }
        else { //we  use djikstras
            vector<int> results = g.dijkstra(deliveryLocations[i]);
            for (int j = 1; j < (deliveryLocations.size() + 1); j++) {
                newGraph.addEdge(i, j, results[j]);
            }
        }
    }
    return newGraph;
}


int main()
{
    int sizeOfCountry = 0;
    int numOfVertices;
    cout << "*********Welcome to Triple A's Delivery service*********" << endl;
    cout << "Enter the Size of your Country(Area in km^2): " << endl;
    cin >> sizeOfCountry;
    if (sizeOfCountry < 1000) {
        numOfVertices = 10000;
    }
    else if (sizeOfCountry < 2500) {
        numOfVertices = 25000;
    }
    else if (sizeOfCountry < 5000) {
        numOfVertices = 50000;
    }
    else if (sizeOfCountry >= 5000) {
        numOfVertices = 100000;
    }
    else {
        cout << "Please input a valid size: " << endl;
    }

    WeightUndirectedGraph g(numOfVertices);
    g.generateGraph();

    int numberOfOrders;
    string input = "";
    string output = "Bellman Ford's ";
    cout << "Your country has " << g.getNumOfVerticies() << " of intersections/locations!" << endl;
    cout << "And your country has a total of " << g.getNumOfEdges() << " roads!" << endl;
    cout << "How many orders would you like to place? (Max of 5): " << endl;
    cin >> numberOfOrders;
    cout << "How would you like to run this program?" << endl;
    cout << "Djikstra's or Bellman's? d/b" << endl;
    cin >> input;
    bool bell = true;
    if (input == "d") {
        bell = false;
        output = "Djikstra's ";
    }
    vector<int> loc;
    for (int i = 0; i < numberOfOrders; i++) {
        loc.push_back(rand() % g.getNumOfVerticies());
    }

    WeightUndirectedGraph q = smallerGraph(loc, g, bell);
    
    
    cout << "Time(Mins) Calculated by " << output << "algorithim: " <<q.shortestTimeToComplete(loc.size()) << endl;

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging men
