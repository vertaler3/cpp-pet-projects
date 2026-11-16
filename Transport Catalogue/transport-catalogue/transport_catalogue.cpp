#include "transport_catalogue.h"

#include <unordered_set>

namespace transport {

void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coordinates) {
    stops_.push_back({name, coordinates});
    const Stop* stop = &stops_.back();
    stop_index_.emplace(stop->name, stop);
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string_view>& stop_names) {
    Bus bus;
    bus.name = name;
    bus.stops.reserve(stop_names.size());

    for (const std::string_view stop_name : stop_names) {
        bus.stops.push_back(FindStop(stop_name));
    }

    buses_.push_back(std::move(bus));
    const Bus* bus_ptr = &buses_.back();
    bus_index_.emplace(bus_ptr->name, bus_ptr);

    for (const Stop* stop : bus_ptr->stops) {
        stop_to_buses_[stop].insert(bus_ptr->name);
    }
}

const TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view name) const {
    const auto it = stop_index_.find(name);
    return it == stop_index_.end() ? nullptr : it->second;
}

const TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view name) const {
    const auto it = bus_index_.find(name);
    return it == bus_index_.end() ? nullptr : it->second;
}

TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    BusInfo info;
    const Bus* bus = FindBus(name);
    if (bus == nullptr) {
        return info;
    }

    info.stops_on_route = bus->stops.size();

    const std::unordered_set<const Stop*> unique_stops(bus->stops.begin(), bus->stops.end());
    info.unique_stops = unique_stops.size();

    double route_length = 0.0;
    for (size_t i = 1; i < bus->stops.size(); ++i) {
        route_length += geo::ComputeDistance(bus->stops[i - 1]->coordinates,
                                             bus->stops[i]->coordinates);
    }
    info.route_length = route_length;

    return info;
}

const std::set<std::string_view>& TransportCatalogue::GetStopInfo(std::string_view name) const {
    static const std::set<std::string_view> empty_set;

    const Stop* stop = FindStop(name);
    if (stop == nullptr) {
        return empty_set;
    }

    const auto it = stop_to_buses_.find(stop);
    if (it != stop_to_buses_.end()) {
        return it->second;
    }

    return empty_set;
}

}
