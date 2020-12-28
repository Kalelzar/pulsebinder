#include <cstdio>
#include <cstring>
#include <iostream>

#include <pulsebind/client.hpp>
#include <pulsebind/device.hpp>
#include <pulsebind/list.hpp>
#include <pulsebind/pulseaudio.hpp>
#include <pulsebind/server.hpp>
#include <pulsebind/sink.hpp>
#include <pulsebind/softwareSink.hpp>
#include <pulsebind/softwareSource.hpp>
#include <pulsebind/source.hpp>

inline List sources;
inline List sinks;
inline List sourceOutputs;
inline List sinkInputs;
inline List clients;
inline char *format = nullptr;
inline bool human = true;

constexpr char *SINK_OR_SOURCE_FORMAT = "~t,~i,~n,~d,~m,~v,~c,~V\n";
constexpr char *SINK_OR_SOURCE_FORMAT_HUMAN = "~t: (~i) - ~d: ~V (~M)\n";
constexpr char *INPUT_OR_OUTPUT_FORMAT = "~t,~i,~C,~s,~n,~d,~m,~v,~c,~V\n";
constexpr char *INPUT_OR_OUTPUT_FORMAT_HUMAN =
    "~t: (~i) - ~d[~n]: ~V (~M) on ~s\n";

inline Pulsebind::Server server;
inline Pulsebind::Device selected;

void listSinks(Pulsebind::PulseAudio &pa) {
  if (!sinks.array)
    sinks = Pulsebind::getSinks(pa);
  char *fmt;
  if (human)
    fmt = SINK_OR_SOURCE_FORMAT_HUMAN;
  else
    fmt = SINK_OR_SOURCE_FORMAT;
  for (size_t i = 0; i < sinks.size; i++) {
    Pulsebind::Device dev =
        Pulsebind::newDevice(pa, Pulsebind::SINK, (void *)listGet(sinks, i));
    Pulsebind::printDevice(pa, dev, fmt);
  }
}

void listSources(Pulsebind::PulseAudio &pa) {
  if (!sources.array)
    sources = Pulsebind::getSources(pa);
  char *fmt;
  if (human)
    fmt = SINK_OR_SOURCE_FORMAT_HUMAN;
  else
    fmt = SINK_OR_SOURCE_FORMAT;
  for (size_t i = 0; i < sources.size; i++) {
    Pulsebind::Device dev = Pulsebind::newDevice(pa, Pulsebind::SOURCE,
                                                 (void *)listGet(sources, i));
    Pulsebind::printDevice(pa, dev, fmt);
  }
}

void listSinkInputs(Pulsebind::PulseAudio &pa) {
  if (!sinkInputs.array)
    sinkInputs = Pulsebind::getSinkInputs(pa);
  char *fmt;
  if (human)
    fmt = INPUT_OR_OUTPUT_FORMAT_HUMAN;
  else
    fmt = INPUT_OR_OUTPUT_FORMAT;
  for (size_t i = 0; i < sinkInputs.size; i++) {
    Pulsebind::Device dev = Pulsebind::newDevice(
        pa, Pulsebind::SINK_INPUT, (void *)listGet(sinkInputs, i));
    Pulsebind::printDevice(pa, dev, fmt);
  }
}

void listSourceOutputs(Pulsebind::PulseAudio &pa) {
  if (!sourceOutputs.array)
    sourceOutputs = Pulsebind::getSourceOutputs(pa);
  char *fmt;
  if (human)
    fmt = INPUT_OR_OUTPUT_FORMAT_HUMAN;
  else
    fmt = INPUT_OR_OUTPUT_FORMAT;
  for (size_t i = 0; i < sourceOutputs.size; i++) {
    Pulsebind::Device dev = Pulsebind::newDevice(
        pa, Pulsebind::SOURCE_OUTPUT, (void *)listGet(sourceOutputs, i));
    Pulsebind::printDevice(pa, dev, fmt);
  }
}

void listAll(Pulsebind::PulseAudio &pa) {
  listSinks(pa);
  listSinkInputs(pa);
  listSources(pa);
  listSourceOutputs(pa);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "No arguments provided." << std::endl;
    return 0;
  }

  Pulsebind::PulseAudio pa = Pulsebind::newPulse("pulsebinder");

  if (pa.state == Pulsebind::ERROR) {
    std::cerr << "Failed to connect to Pulseaudio server." << std::endl;
    return 1;
  }

  server = Pulsebind::getServerInfo(pa);

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
  // .hh Query
  // human                | human-readable
  // machine              | machine-readable
  // volume               | Return the avg volume between all channels
  // channel-volume       | Return the audio for all channels
  // is-mute              | Return mute status
  // print                | Print all info available for selected device

  selected = {Pulsebind::UNDEFINED};

  for (int i = 1; i < argc; i++) {
    char *command = argv[i];
    size_t command_len = strlen(command);
    if (strncmp(command, "list", 4) == 0) {
      if (command_len == 4) {
        listAll(pa);
      } else if (strcmp(command + 4, "-sinks") == 0) {
        listSinks(pa);
      } else if (strcmp(command + 4, "-sources") == 0) {
        listSources(pa);
      } else if (strcmp(command + 4, "-sink-inputs") == 0) {
        listSinkInputs(pa);
      } else if (strcmp(command + 4, "-source-outputs") == 0) {
        listSourceOutputs(pa);
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
          if (!sinks.array)
            sinks = Pulsebind::getSinks(pa);

          Pulsebind::Sink *hws = Pulsebind::getSinkById(sinks, id);
          if (hws) {
            selected.device.sink = hws;
            selected.type = Pulsebind::SINK;
          } else {
            if (!sinkInputs.array)
              sinkInputs = Pulsebind::getSinkInputs(pa);
            Pulsebind::SinkInput *sws =
                Pulsebind::getSinkInputById(sinkInputs, id);
            if (sws) {
              selected.device.sinkInput = sws;
              selected.type = Pulsebind::SINK_INPUT;
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
          if (!sinks.array)
            sinks = Pulsebind::getSinks(pa);

          Pulsebind::Sink *hws = Pulsebind::getSinkByName(sinks, arg);
          if (hws) {
            selected.device.sink = hws;
            selected.type = Pulsebind::SINK;
          } else {
            if (!sinkInputs.array)
              sinkInputs = Pulsebind::getSinkInputs(pa);
            Pulsebind::SinkInput *sws =
                Pulsebind::getSinkInputByName(sinkInputs, arg);
            if (sws) {
              selected.device.sinkInput = sws;
              selected.type = Pulsebind::SINK_INPUT;
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
          if (!sinks.array)
            sinks = Pulsebind::getSinks(pa);

          Pulsebind::Sink *hws = Pulsebind::getSinkByDescription(sinks, arg);
          if (hws) {
            selected.device.sink = hws;
            selected.type = Pulsebind::SINK;
          } else {
            if (!sinkInputs.array)
              sinkInputs = Pulsebind::getSinkInputs(pa);
            if (!clients.array)
              clients = Pulsebind::getClients(pa);
            Pulsebind::SinkInput *sws =
                Pulsebind::getSinkInputByDescription(sinkInputs, clients, arg);
            if (sws) {
              selected.device.sinkInput = sws;
              selected.type = Pulsebind::SINK_INPUT;
            } else {
              std::cerr << "No sink with fancy name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 8, "default") == 0) {
          if (!sinks.array)
            sinks = Pulsebind::getSinks(pa);
          Pulsebind::Sink *hws =
              Pulsebind::getSinkByName(sinks, server.defaultSinkName);
          if (hws) {
            selected.device.sink = hws;
            selected.type = Pulsebind::SINK;
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
          if (!sources.array)
            sources = Pulsebind::getSources(pa);

          Pulsebind::Source *hws = Pulsebind::getSourceById(sources, id);
          if (hws) {
            selected.device.source = hws;
            selected.type = Pulsebind::SOURCE;
          } else {
            if (!sourceOutputs.array)
              sourceOutputs = Pulsebind::getSourceOutputs(pa);
            Pulsebind::SourceOutput *sws =
                Pulsebind::getSourceOutputById(sourceOutputs, id);
            if (sws) {
              selected.device.sourceOutput = sws;
              selected.type = Pulsebind::SOURCE_OUTPUT;
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
          if (!sources.array)
            sources = Pulsebind::getSources(pa);

          Pulsebind::Source *hws = Pulsebind::getSourceByName(sources, arg);
          if (hws) {
            selected.device.source = hws;
            selected.type = Pulsebind::SOURCE_OUTPUT;
          } else {
            if (!sourceOutputs.array)
              sourceOutputs = Pulsebind::getSourceOutputs(pa);
            Pulsebind::SourceOutput *sws =
                Pulsebind::getSourceOutputByName(sourceOutputs, arg);
            if (sws) {
              selected.device.sourceOutput = sws;
              selected.type = Pulsebind::SOURCE_OUTPUT;
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
          if (!sources.array)
            sources = Pulsebind::getSources(pa);

          Pulsebind::Source *hws =
              Pulsebind::getSourceByDescription(sources, arg);
          if (hws) {
            selected.device.source = hws;
            selected.type = Pulsebind::SOURCE;
          } else {
            if (!sourceOutputs.array)
              sourceOutputs = Pulsebind::getSourceOutputs(pa);
            if (!clients.array)
              clients = Pulsebind::getClients(pa);
            Pulsebind::SourceOutput *sws =
                Pulsebind::getSourceOutputByDescription(sourceOutputs, clients,
                                                        arg);
            if (sws) {
              selected.device.sourceOutput = sws;
              selected.type = Pulsebind::SOURCE_OUTPUT;
            } else {
              std::cerr << "No source with fancy name " << arg << " found."
                        << std::endl;
              break;
            }
          }
        } else if (strcmp(command + 10, "default") == 0) {
          if (!sources.array)
            sources = Pulsebind::getSources(pa);
          Pulsebind::Source *hws =
              Pulsebind::getSourceByName(sources, server.defaultSourceName);
          if (hws) {
            selected.device.source = hws;
            selected.type = Pulsebind::SOURCE;
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
      if (selected.type != Pulsebind::UNDEFINED) {
        if (selected.type == Pulsebind::SINK ||
            selected.type == Pulsebind::SOURCE)
          std::cout << selected.device.sink->volumePercent << std::endl;
        else
          std::cout << selected.device.sinkInput->volumePercent << std::endl;
      } else {
        if (!sinks.array)
          sinks = Pulsebind::getSinks(pa);
        Pulsebind::Sink *hws =
            Pulsebind::getSinkByName(sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        std::cout << hws->volumePercent << std::endl;
      }
    } else if (strcmp(command, "channel-volume") == 0) {
      if (selected.type != Pulsebind::UNDEFINED) {
        pa_cvolume vol;
        if (selected.type == Pulsebind::SINK ||
            selected.type == Pulsebind::SOURCE)
          vol = selected.device.sink->volume;
        else
          vol = selected.device.sinkInput->volume;
        for (uint8_t i = 0; i < vol.channels; i++) {
          std::cout << Pulsebind::normalizeVolume(vol.values[i]) << " ";
        }
        std::cout << std::endl;
      } else {
        pa_cvolume vol;
        if (!sinks.array)
          sinks = Pulsebind::getSinks(pa);
        Pulsebind::Sink *hws =
            Pulsebind::getSinkByName(sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        vol = hws->volume;
        for (uint8_t i = 0; i < vol.channels; i++) {
          std::cout << Pulsebind::normalizeVolume(vol.values[i]) << " ";
        }
        std::cout << std::endl;
      }
    } else if (strcmp(command, "is-mute") == 0) {
      std::cout << std::boolalpha;
      if (selected.type != Pulsebind::UNDEFINED) {
        if (selected.type == Pulsebind::SINK ||
            selected.type == Pulsebind::SOURCE)
          std::cout << selected.device.sink->mute << std::endl;
        else
          std::cout << selected.device.sinkInput->mute << std::endl;
      } else {
        if (!sinks.array)
          sinks = Pulsebind::getSinks(pa);
        Pulsebind::Sink *hws =
            Pulsebind::getSinkByName(sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        std::cout << hws->mute << std::endl;
      }
    } else if (strcmp(command, "print") == 0) {

      if (selected.type != Pulsebind::UNDEFINED) {
        char *pformat;
        if (format == nullptr) {
          switch (selected.type) {
          case Pulsebind::SINK:
          case Pulsebind::SOURCE:
            if (human) {
              pformat = SINK_OR_SOURCE_FORMAT_HUMAN;
            } else {
              pformat = SINK_OR_SOURCE_FORMAT;
            }
            break;
          case Pulsebind::SINK_INPUT:
          case Pulsebind::SOURCE_OUTPUT:
            if (human) {
              pformat = INPUT_OR_OUTPUT_FORMAT_HUMAN;
            } else {
              pformat = INPUT_OR_OUTPUT_FORMAT;
            }
            break;
          case Pulsebind::UNDEFINED:
            pformat = "UNDEFINED SELECTION";
            break;
          }
        }
        Pulsebind::printDevice(pa, selected, pformat);
      } else {
        if (!sinks.array)
          sinks = Pulsebind::getSinks(pa);
        Pulsebind::Sink *hws =
            Pulsebind::getSinkByName(sinks, server.defaultSinkName);
        if (!hws) {
          std::cerr << "Failed to get default sink. Aborting...";
          break;
        }
        Pulsebind::Device d =
            Pulsebind::newDevice(pa, Pulsebind::SINK, (void *)hws);
        Pulsebind::printDevice(pa, d,
                               human ? SINK_OR_SOURCE_FORMAT_HUMAN
                                     : SINK_OR_SOURCE_FORMAT);
      }
    } else if (strcmp(command, "human") == 0) {
      human = true;
    } else if (strcmp(command, "machine") == 0) {
      human = false;
    } else {
      std::cerr << "Unrecognized command: " << command << std::endl;
      break;
    }
  }

  if (sinks.array)
    Pulsebind::freeSinks(sinks);
  if (sources.array)
    Pulsebind::freeSources(sources);
  if (sinkInputs.array)
    Pulsebind::freeSinkInputs(sinkInputs);
  if (sourceOutputs.array)
    Pulsebind::freeSourceOutputs(sourceOutputs);
  if (clients.array)
    Pulsebind::freeClients(clients);

  Pulsebind::deleteServer(server);
  Pulsebind::deletePulse(pa);
  return 0;
}
