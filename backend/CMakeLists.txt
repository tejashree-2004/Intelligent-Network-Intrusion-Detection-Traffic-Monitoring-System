cmake_minimum_required(VERSION 3.16)
project(network_intrusion_detection_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_PCAP "Build with libpcap packet capture support" OFF)

add_executable(nids_service
    main.cpp
    packet-capture/MockPacketCapture.cpp
    parser/PacketParser.cpp
    flow-engine/FlowAggregator.cpp
    detection-engine/RuleEngine.cpp
    common/Database.cpp
)

target_include_directories(nids_service PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(ENABLE_PCAP)
    find_library(PCAP_LIBRARY pcap REQUIRED)
    target_sources(nids_service PRIVATE packet-capture/PcapPacketCapture.cpp)
    target_compile_definitions(nids_service PRIVATE ENABLE_PCAP=1)
    target_link_libraries(nids_service PRIVATE ${PCAP_LIBRARY})
endif()

if(WIN32)
    target_compile_definitions(nids_service PRIVATE NOMINMAX)
endif()
