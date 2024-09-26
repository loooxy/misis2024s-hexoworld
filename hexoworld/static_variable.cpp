#include <hexoworld/hexoworld.hpp>

const double Hexoworld::PRECISION_DBL_CALC = 0.0001;
std::unordered_map<std::uintptr_t, std::thread::id> Hexoworld::OneThreadController::who_blocked;
std::unordered_map<std::uintptr_t, std::unique_ptr<std::mutex>> Hexoworld::OneThreadController::mutexs;
std::map<uint32_t, uint32_t> Hexoworld::IdType::cnts;