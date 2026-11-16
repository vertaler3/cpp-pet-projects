#pragma once

#include <cstddef>
#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

namespace transport {

class TransportCatalogue {
public:
    struct Stop {
        std::string name;
        geo::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
    };

    struct BusInfo {
        size_t stops_on_route = 0;
        size_t unique_stops = 0;
        double route_length = 0.0;
    };

    void AddStop(const std::string& name, geo::Coordinates coordinates);

    void AddBus(const std::string& name, const std::vector<std::string_view>& stop_names);

    const Stop* FindStop(std::string_view name) const;

    const Bus* FindBus(std::string_view name) const;

    BusInfo GetBusInfo(std::string_view name) const;

    const std::set<std::string_view>& GetStopInfo(std::string_view name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stop_index_;
    std::unordered_map<std::string_view, const Bus*> bus_index_;

    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_buses_;
};

}
