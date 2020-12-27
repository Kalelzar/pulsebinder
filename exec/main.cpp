#include <cstdio>
#include <cstring>
#include <iostream>

#include <pulsebind/client.hpp>
#include <pulsebind/list.hpp>
#include <pulsebind/pulseaudio.hpp>
#include <pulsebind/server.hpp>
#include <pulsebind/sink.hpp>
#include <pulsebind/softwareSink.hpp>
#include <pulsebind/softwareSource.hpp>
#include <pulsebind/source.hpp>

inline List hw_sources;
inline List hw_sinks;
inline List sw_sources;
inline List sw_sinks;
inline List clients;
inline Pulsebind::Server server;

enum DeviceType { HW_SINK, SW_SINK, HW_SOURCE, SW_SOURCE, UNDEFINED };

struct Device {
  DeviceType type = UNDEFINED;
  union {
    Pulsebind::HardwareSink *hw_sink;
    Pulsebind::SoftwareSink *sw_sink;
    Pulsebind::HardwareSource *hw_source;
    Pulsebind::SoftwareSource *sw_source;
  } device;
};

inline Device selected;

void print_sw_sink(Pulsebind::PulseAudio &pa, Pulsebind::SoftwareSink *sws) {
  std::cout << "SINK_INPUT,";
  // ID
  std::cout << sws->id << ",";
  // NAME
  std::cout << sws->name << ",";
  // FANCYNAME
  Pulsebind::Client *client = Pulsebind::get_client_by_id(clients, sws->client);
  if (client)
    std::cout << client->name;
  std::cout << ",";
  // CLIENTID
  std::cout << sws->client << ",";
  // SINKID
  std::cout << sws->sink << ",";
  // MUTE
  std::cout << sws->mute << ",";
  // VOLUMEPERCENT
  std::cout << sws->volumePercent << ",";
  // CHANNELS
  std::cout << (int)sws->volume.channels << ",";
  // CHANNEL_VOLUMES
  for (uint8_t j = 0; j < sws->volume.channels; j++) {
    std::cout << Pulsebind::normalize_volume(sws->volume.values[j]) << " ";
  }
  std::cout << std::endl;
}

void list_sw_sinks(Pulsebind::PulseAudio &pa) {
  if (!sw_sinks.array) {
    sw_sinks = Pulsebind::get_software_sinks(pa);
  }
  std::cout << "TYPE,ID,NAME,FNAME,CID,SINK,MUTE,V%,CH,"
               "CH%"
            << std::endl;
  if (sw_sinks.size > 0 && !clients.array) {
    clients = Pulsebind::get_clients(pa);
  }

  for (size_t i = 0; i < sw_sinks.size; i++) {
    Pulsebind::SoftwareSink *sws =
        (Pulsebind::SoftwareSink *)list_get(sw_sinks, i);
    print_sw_sink(pa, sws);
  }
}

void print_hw_sink(Pulsebind::HardwareSink *hws) {
  std::cout << "SINK,";
  // ID
  std::cout << hws->id << ",";
  // NAME
  std::cout << hws->name << ",";
  // FANCYNAME
  std::cout << hws->description << ",";
  // MUTE
  std::cout << std::boolalpha << hws->mute << ",";
  // VOLUMEPERCENT
  std::cout << hws->volumePercent << ",";
  // CHANNELS
  std::cout << (int)hws->volume.channels << ",";
  for (uint8_t j = 0; j < hws->volume.channels; j++) {
    std::cout << Pulsebind::normalize_volume(hws->volume.values[j]) << " ";
  }
  std::cout << std::endl;
}

void list_hw_sinks(Pulsebind::PulseAudio &pa) {
  if (!hw_sinks.array) {
    hw_sinks = Pulsebind::get_hardware_sinks(pa);
  }
  std::cout << "TYPE,ID,NAME,FNAME,MUTE,V%,CH,CH%" << std::endl;
  for (size_t i = 0; i < hw_sinks.size; i++) {
    Pulsebind::HardwareSink *hws =
        (Pulsebind::HardwareSink *)list_get(hw_sinks, i);
    print_hw_sink(hws);
  }
}

void print_sw_source(Pulsebind::PulseAudio &pa,
                     Pulsebind::SoftwareSource *sws) {
  std::cout << "SOURCE_OUTPUT,";
  // ID
  std::cout << sws->id << ",";
  // NAME
  std::cout << sws->name << ",";
  // FANCYNAME
  Pulsebind::Client *client = Pulsebind::get_client_by_id(clients, sws->client);
  if (client)
    std::cout << client->name;
  std::cout << ",";
  // CLIENTID
  std::cout << sws->client << ",";
  // SOURCEID
  std::cout << sws->source << ",";
  // MUTE
  std::cout << sws->mute << ",";
  // VOLUMEPERCENT
  std::cout << sws->volumePercent << ",";
  // CHANNELS
  std::cout << (int)sws->volume.channels << ",";
  // CHANNEL_VOLUMES
  for (uint8_t j = 0; j < sws->volume.channels; j++) {
    std::cout << Pulsebind::normalize_volume(sws->volume.values[j]) << " ";
  }
  std::cout << std::endl;
}

void list_sw_sources(Pulsebind::PulseAudio &pa) {
  if (!sw_sources.array) {
    sw_sources = Pulsebind::get_software_sources(pa);
  }
  std::cout << "TYPE,ID,NAME,FNAME,CID,SOURCE,MUTE,V%,CH,"
               "CH%"
            << std::endl;
  if (sw_sources.size > 0 && !clients.array) {
    clients = Pulsebind::get_clients(pa);
  }
  for (size_t i = 0; i < sw_sources.size; i++) {
    Pulsebind::SoftwareSource *sws =
        (Pulsebind::SoftwareSource *)list_get(sw_sources, i);
    print_sw_source(pa, sws);
  }
}

void print_hw_source(Pulsebind::HardwareSource *hws) {
  std::cout << "SOURCE,";
  // ID
  std::cout << hws->id << ",";
  // NAME
  std::cout << hws->name << ",";
  // FANCYNAME
  std::cout << hws->description << ",";
  // MUTE
  std::cout << std::boolalpha << hws->mute << ",";
  // VOLUMEPERCENT
  std::cout << hws->volumePercent << ",";
  // CHANNELS
  std::cout << (int)hws->volume.channels << ",";
  for (uint8_t j = 0; j < hws->volume.channels; j++) {
    std::cout << Pulsebind::normalize_volume(hws->volume.values[j]) << " ";
  }
  std::cout << std::endl;
}

void list_hw_sources(Pulsebind::PulseAudio &pa) {
  if (!hw_sources.array) {
    hw_sources = Pulsebind::get_hardware_sources(pa);
  }
  std::cout
      << "TYPE,ID,NAME,FNAME,MUTE,V%,CH,CH%"
      << std::endl;
  for (size_t i = 0; i < hw_sources.size; i++) {
    Pulsebind::HardwareSource *hws =
        (Pulsebind::HardwareSource *)list_get(hw_sources, i);
    print_hw_source(hws);
  }
}

void list_all(Pulsebind::PulseAudio &pa) {
  list_hw_sinks(pa);
  list_sw_sinks(pa);
  list_hw_sources(pa);
  list_sw_sources(pa);
}

void print_device(Pulsebind::PulseAudio &pa, Device &d) {
  switch (d.type) {
  case DeviceType::SW_SINK:
    print_sw_sink(pa, d.device.sw_sink);
    break;
  case DeviceType::SW_SOURCE:
    print_sw_source(pa, d.device.sw_source);
    break;
  case DeviceType::HW_SOURCE:
    print_hw_source(d.device.hw_source);
    break;
  case DeviceType::HW_SINK:
    print_hw_sink(d.device.hw_sink);
    break;
  case DeviceType::UNDEFINED:
    if (!hw_sinks.array)
      hw_sinks = Pulsebind::get_hardware_sinks(pa);
    Pulsebind::HardwareSink *hws =
        Pulsebind::get_hardware_sink_by_name(hw_sinks, server.defaultSinkName);
    print_hw_sink(hws);
    break;
  }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "No arguments provided." << std::endl;
    return 0;
  }

  Pulsebind::PulseAudio pa = Pulsebind::new_pulse("pulsebinder");

  if (pa.state == Pulsebind::ERROR) {
    std::cerr << "Failed to connect to Pulseaudio server." << std::endl;
    return 1;
  }

  server = Pulsebind::get_server_info(pa);

  // .hh List
  // list                 | List all sources/sinks V
  // list-sinks           | List all sinks V
  // list-sources         | List all sources V
  // list-sink-inputs     | List all sink inputs C
  // list-source-outputs  | List all source outputs V
  // default-sink         | Get default sink
  // default-source       | Get default source
  // .hh Selection        |
  // sink-by-id           | Select sink by id
  // sink-by-name         | Select sink by name
  // sink-by-fancy-name   | Select sink by fancy name
  // source-by-id         | Select source by id
  // source-by-name       | Select source by name
  // source-by-fancy-name | Select source by fancy name
  // sink-by-default      | Select default sink (default)
  // source-by-default    | Select default sourcexs
  // .hh Query            |
  // volume               | Return the avg volume between all channels
  // channel-volume       | Return the audio for all channels
  // is-mute              | Return mute status
  // print                | Print all info available for selected device

  for (int i = 1; i < argc; i++) {
    char *command = argv[i];
    size_t command_len = strlen(command);
    if (strncmp(command, "list", 4) == 0) {
      if (command_len == 4) {
        list_all(pa);
      } else if (strcmp(command + 4, "-sinks") == 0) {
        list_hw_sinks(pa);
      } else if (strcmp(command + 4, "-sources") == 0) {
        list_hw_sources(pa);
      } else if (strcmp(command + 4, "-sink-inputs") == 0) {
        list_sw_sinks(pa);
      } else if (strcmp(command + 4, "-source-outputs") == 0) {
        list_sw_sources(pa);
      } else {
        std::cerr << "Unrecognized command: " << command << std::endl;
        break;
      }
    } else if (strncmp(command, "default", 7) == 0) {
      if (strcmp(command + 7, "-sink") == 0) {
        std::cout << "Default Sink," << server.defaultSinkName << std::endl;
      } else if (strcmp(command + 7, "-source") == 0) {
        std::cout << "Default Source," << server.defaultSourceName << std::endl;
      } else {
        std::cerr << "Unrecognized command: " << command << std::endl;
        break;
      }
    } else if (strncmp(command, "sink", 4) == 0) {
      if (strncmp(command + 4, "-by-", 4) == 0) {
        if (strcmp(command + 8, "id") == 0) {
          if (i + 1 >= argc) {
            std::cerr << "You need to provide a numeric argument for the ID."
                      << std::endl;
            break;
          }
          char *arg = argv[++i];
          uint32_t id;
          if (sscanf(arg, "%d", &id) != 1) {
            std::cerr << "Failed to parse ID." << std::endl;
            break;
          }
          if (!hw_sinks.array)
            hw_sinks = Pulsebind::get_hardware_sinks(pa);

          Pulsebind::HardwareSink *hws =
              Pulsebind::get_hardware_sink_by_id(hw_sinks, id);
          if (hws) {
            selected.device.hw_sink = hws;
            selected.type = HW_SINK;
          } else {
            if (!sw_sinks.array)
              sw_sinks = Pulsebind::get_software_sinks(pa);
            Pulsebind::SoftwareSink *sws =
                Pulsebind::get_software_sink_by_id(sw_sinks, id);
            if (sws) {
              selected.device.sw_sink = sws;
              selected.type = SW_SINK;
            } else {
              std::cerr << "No sink with id " << id << " found." << std::endl;
              break;
            }
          }

        } else if (strcmp(command + 8, "name") == 0) {
          if (i + 1 >= argc) {
            std::cerr << "You need to provide an argument for the name."
                      << std::endl;
            break;
          }
          char *arg = argv[++i];
          if (!hw_sinks.array)
            hw_sinks = Pulsebind::get_hardware_sinks(pa);

          Pulsebind::HardwareSink *hws =
              Pulsebind::get_hardware_sink_by_name(hw_sinks, arg);
          if (hws) {
            selected.device.hw_sink = hws;
            selected.type = HW_SINK;
          } else {
            if (!sw_sinks.array)
              sw_sinks = Pulsebind::get_software_sinks(pa);
            Pulsebind::SoftwareSink *sws =
                Pulsebind::get_software_sink_by_name(sw_sinks, arg);
            if (sws) {
              selected.device.sw_sink = sws;
              selected.type = SW_SINK;
            } else {
              std::cerr << "No sink with name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 8, "fancy-name") == 0) {
          if (i + 1 >= argc) {
            std::cerr << "You need to provide an argument for the fancy name."
                      << std::endl;
            break;
          }

          char *arg = argv[++i];
          if (!hw_sinks.array)
            hw_sinks = Pulsebind::get_hardware_sinks(pa);

          Pulsebind::HardwareSink *hws =
              Pulsebind::get_hardware_sink_by_description(hw_sinks, arg);
          if (hws) {
            selected.device.hw_sink = hws;
            selected.type = HW_SINK;
          } else {
            if (!sw_sinks.array)
              sw_sinks = Pulsebind::get_software_sinks(pa);
            if (!clients.array)
              clients = Pulsebind::get_clients(pa);
            Pulsebind::SoftwareSink *sws =
                Pulsebind::get_software_sink_by_description(sw_sinks, clients,
                                                            arg);
            if (sws) {
              selected.device.sw_sink = sws;
              selected.type = SW_SINK;
            } else {
              std::cerr << "No sink with fancy name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 8, "default") == 0) {
          if (!hw_sinks.array)
            hw_sinks = Pulsebind::get_hardware_sinks(pa);
          Pulsebind::HardwareSink *hws = Pulsebind::get_hardware_sink_by_name(
              hw_sinks, server.defaultSinkName);
          if (hws) {
            selected.device.hw_sink = hws;
            selected.type = HW_SINK;
          } else {
            std::cerr << "Failed to find default sink. Aborting..."
                      << std::endl;
            break;
          }
        } else {
          std::cerr << "Unrecognized command: " << command << std::endl;
          break;
        }
      } else {
        std::cerr << "Unrecognized command: " << command << std::endl;
        break;
      }
    } else if (strncmp(command, "source", 6) == 0) {
      if (strncmp(command + 6, "-by-", 4) == 0) {
        if (strcmp(command + 10, "id") == 0) {
          if (i + 1 >= argc) {
            std::cerr << "You need to provide a numeric argument for the ID."
                      << std::endl;
            break;
          }
          char *arg = argv[++i];
          uint32_t id;
          if (sscanf(arg, "%d", &id) != 1) {
            std::cerr << "Failed to parse ID." << std::endl;
            break;
          }
          if (!hw_sources.array)
            hw_sources = Pulsebind::get_hardware_sources(pa);

          Pulsebind::HardwareSource *hws =
              Pulsebind::get_hardware_source_by_id(hw_sources, id);
          if (hws) {
            selected.device.hw_source = hws;
            selected.type = HW_SOURCE;
          } else {
            if (!sw_sources.array)
              sw_sources = Pulsebind::get_software_sources(pa);
            Pulsebind::SoftwareSource *sws =
                Pulsebind::get_software_source_by_id(sw_sources, id);
            if (sws) {
              selected.device.sw_source = sws;
              selected.type = SW_SOURCE;
            } else {
              std::cerr << "No source with id " << id << " found." << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 10, "name") == 0) {

          if (i + 1 >= argc) {
            std::cerr << "You need to provide an argument for the name."
                      << std::endl;
            break;
          }
          char *arg = argv[++i];
          if (!hw_sources.array)
            hw_sources = Pulsebind::get_hardware_sources(pa);

          Pulsebind::HardwareSource *hws =
              Pulsebind::get_hardware_source_by_name(hw_sources, arg);
          if (hws) {
            selected.device.hw_source = hws;
            selected.type = HW_SOURCE;
          } else {
            if (!sw_sources.array)
              sw_sources = Pulsebind::get_software_sources(pa);
            Pulsebind::SoftwareSource *sws =
                Pulsebind::get_software_source_by_name(sw_sources, arg);
            if (sws) {
              selected.device.sw_source = sws;
              selected.type = SW_SOURCE;
            } else {
              std::cerr << "No source with name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 10, "fancy-name") == 0) {
          if (i + 1 >= argc) {
            std::cerr << "You need to provide an argument for the name."
                      << std::endl;
            break;
          }
          char *arg = argv[++i];
          if (!hw_sources.array)
            hw_sources = Pulsebind::get_hardware_sources(pa);

          Pulsebind::HardwareSource *hws =
              Pulsebind::get_hardware_source_by_description(hw_sources, arg);
          if (hws) {
            selected.device.hw_source = hws;
            selected.type = HW_SOURCE;
          } else {
            if (!sw_sources.array)
              sw_sources = Pulsebind::get_software_sources(pa);
            if (!clients.array)
              clients = Pulsebind::get_clients(pa);
            Pulsebind::SoftwareSource *sws =
                Pulsebind::get_software_source_by_description(sw_sources,
                                                              clients, arg);
            if (sws) {
              selected.device.sw_source = sws;
              selected.type = SW_SOURCE;
            } else {
              std::cerr << "No source with fancy name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 10, "default") == 0) {
          if (!hw_sources.array)
            hw_sources = Pulsebind::get_hardware_sources(pa);
          Pulsebind::HardwareSource *hws =
              Pulsebind::get_hardware_source_by_name(hw_sources,
                                                     server.defaultSourceName);
          if (hws) {
            selected.device.hw_source = hws;
            selected.type = HW_SOURCE;
          } else {
            std::cerr << "Failed to find default source. Aborting..."
                      << std::endl;
            break;
          }
        } else {
          std::cerr << "Unrecognized command: " << command << std::endl;
          break;
        }
      } else {
        std::cerr << "Unrecognized command: " << command << std::endl;
        break;
      }
    } else if (strcmp(command, "volume") == 0) {
      if (selected.type != UNDEFINED) {
        if (selected.type == HW_SINK || selected.type == HW_SOURCE)
          std::cout << selected.device.hw_sink->volumePercent << std::endl;
        else
          std::cout << selected.device.sw_sink->volumePercent << std::endl;
      } else {
        if (!hw_sinks.array)
          hw_sinks = Pulsebind::get_hardware_sinks(pa);
        Pulsebind::HardwareSink *hws = Pulsebind::get_hardware_sink_by_name(
            hw_sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        std::cout << hws->volumePercent << std::endl;
      }
    } else if (strcmp(command, "channel-volume") == 0) {
      if (selected.type != UNDEFINED) {
        pa_cvolume vol;
        if (selected.type == HW_SINK || selected.type == HW_SOURCE)
          vol = selected.device.hw_sink->volume;
        else
          vol = selected.device.sw_sink->volume;
        for (uint8_t i = 0; i < vol.channels; i++) {
          std::cout << Pulsebind::normalize_volume(vol.values[i]) << " ";
        }
        std::cout << std::endl;
      } else {
        pa_cvolume vol;
        if (!hw_sinks.array)
          hw_sinks = Pulsebind::get_hardware_sinks(pa);
        Pulsebind::HardwareSink *hws = Pulsebind::get_hardware_sink_by_name(
            hw_sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        vol = hws->volume;
        for (uint8_t i = 0; i < vol.channels; i++) {
          std::cout << Pulsebind::normalize_volume(vol.values[i]) << " ";
        }
        std::cout << std::endl;
      }
    } else if (strcmp(command, "is-mute") == 0) {
      std::cout << std::boolalpha;
      if (selected.type != UNDEFINED) {
        if (selected.type == HW_SINK || selected.type == HW_SOURCE)
          std::cout << selected.device.hw_sink->mute << std::endl;
        else
          std::cout << selected.device.sw_sink->mute << std::endl;
      } else {
        if (!hw_sinks.array)
          hw_sinks = Pulsebind::get_hardware_sinks(pa);
        Pulsebind::HardwareSink *hws = Pulsebind::get_hardware_sink_by_name(
            hw_sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        std::cout << hws->mute << std::endl;
      }
    } else if (strcmp(command, "print") == 0) {
      print_device(pa, selected);
    } else {
      std::cerr << "Unrecognized command: " << command << std::endl;
      break;
    }
  }

  if (hw_sinks.array)
    Pulsebind::free_hardware_sinks(hw_sinks);
  if (hw_sources.array)
    Pulsebind::free_hardware_sources(hw_sources);
  if (sw_sinks.array)
    Pulsebind::free_software_sinks(sw_sinks);
  if (sw_sources.array)
    Pulsebind::free_software_sources(sw_sources);
  if (clients.array)
    Pulsebind::free_clients(clients);

  Pulsebind::delete_server(server);
  Pulsebind::delete_pulse(pa);
  return 0;
}
