#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <cmath>
#include <fstream>
#include <future>
#include <map>

enum class TaskType {
    Sin,
    SquareRoot,
    Power
};

struct Task {
    TaskType type;
    double argument;
    size_t id;
};

template<typename T>
class Server {
public:
    void start() {
        running = true;
        server_thread = std::thread(&Server::run, this);
    }

    void stop() {
        running = false;
        if (server_thread.joinable())
            server_thread.join();
    }

    size_t add_task(Task task) {
        std::lock_guard<std::mutex> lock(mutex);
        task.id = next_id++;
        tasks.push(task);
        return task.id;
    }

    std::future<T> request_result(size_t id_res) {
        std::lock_guard<std::mutex> lock(mutex);
        std::future<T> future = results[id_res].get_future();
        return future;
    }

private:
    std::thread server_thread;
    std::queue<Task> tasks;
    std::map<size_t, std::promise<T>> results;
    std::mutex mutex;
    size_t next_id = 0;
    bool running;

    void run() {
        while (running) {
            Task task;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (tasks.empty())
                    continue;
                task = tasks.front();
                tasks.pop();
            }

            T result;
            switch (task.type) {
                case TaskType::Sin:
                    result = static_cast<T>(sin(task.argument));
                    break;
                case TaskType::SquareRoot:
                    result = static_cast<T>(sqrt(task.argument));
                    break;
                case TaskType::Power:
                    result = static_cast<T>(pow(task.argument, 2));
                    break;
            }
            {
                std::lock_guard<std::mutex> lock(mutex);
                results[task.id].set_value(result);
            }
        }
    }
};

template<typename T>
void client(Server<T>& server, TaskType type, size_t N, const std::string filename) {
    std::ofstream file(filename);
    for (size_t i = 0; i < N; ++i) {
        T argument = 1.0 + i;
        size_t id = server.add_task({type, argument, 0});
        std::future<T> result_future = server.request_result(id);
        T result = result_future.get();
        file << argument << " = " << result << "\n";
    }
    file.close();
}

int main() {
    Server<double> server;
    server.start();
    std::thread client1(client<double>, std::ref(server), TaskType::Sin, 10000, "sin_results.txt");
    std::thread client2(client<double>, std::ref(server), TaskType::SquareRoot, 10000, "sqrt_results.txt");
    std::thread client3(client<double>, std::ref(server), TaskType::Power, 10000, "power_results.txt");
    client1.join();
    client2.join();
    client3.join();
    server.stop();
    return 0;
}