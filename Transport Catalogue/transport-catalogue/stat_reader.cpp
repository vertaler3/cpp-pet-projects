#include "stat_reader.h"

#include <iomanip>

namespace stat_reader {

void PrintBusInfo(const transport::TransportCatalogue& transport_catalogue,
                  std::string_view name,
                  std::ostream& output) {
    if (transport_catalogue.FindBus(name) == nullptr) {
        output << "Bus " << name << ": not found\n";
        return;
    }

    const auto info = transport_catalogue.GetBusInfo(name);
    output << std::setprecision(6) << "Bus " << name << ": " << info.stops_on_route
           << " stops on route, " << info.unique_stops << " unique stops, "
           << info.route_length << " route length\n";
}

void PrintStopInfo(const transport::TransportCatalogue& transport_catalogue,
                   std::string_view name,
                   std::ostream& output) {
    if (transport_catalogue.FindStop(name) == nullptr) {
        output << "Stop " << name << ": not found\n";
        return;
    }

    const auto& buses = transport_catalogue.GetStopInfo(name);
    if (buses.empty()) {
        output << "Stop " << name << ": no buses\n";
    } else {
        output << "Stop " << name << ": buses ";
        for (const std::string_view bus : buses) {
            output << bus << ' ';
        }
        output << '\n';
    }
}

void ParseAndPrintStat(const transport::TransportCatalogue& transport_catalogue,
                       std::string_view request,
                       std::ostream& output) {
    const size_t space_pos = request.find(' ');
    const std::string_view command = request.substr(0, space_pos);
    const std::string_view name = request.substr(space_pos + 1);

    if (command == "Bus") {
        PrintBusInfo(transport_catalogue, name, output);
    }

    if (command == "Stop") {
        PrintStopInfo(transport_catalogue, name, output);
    }
}

}
