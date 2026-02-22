# 📑 Table of Contents

- [Overview](#esp32--hivemq-cloud-mqtt-esp-idf)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Topic Structure](#topic-structure)
- [Implementation Overview](#implementation-overview)
  - [WiFi Layer](#wifi-layer)
  - [Secure MQTT Layer](#secure-mqtt-layer)
  - [Authentication & Authorization](#authentication--authorization)
  - [FreeRTOS Architecture](#freertos-architecture)
- [Project Structure](#project-structure)
- [Authentication](#authentication)
- [Authorization](#authorization)
- [Security Model](#security-model)
- [What This Project Demonstrates](#what-this-project-demonstrates)
- [License](#license)

# ESP32 + HiveMQ Cloud MQTT (ESP-IDF)

Secure MQTT communication between an ESP32 and HiveMQ Cloud using the ESP-IDF framework.

This project demonstrates a complete and secure IoT communication pipeline including:

- WiFi connectivity management
- MQTT over TLS (Port 8883)
- Username/Password authentication
- Access Control Lists (ACL)
- FreeRTOS-based task architecture
- Scalable topic design for multi-device environments

# Features

- Secure MQTT connection (`mqtts://`)
- TLS server validation using embedded CA certificate
- Credential-based authentication (HiveMQ Cloud)
- Topic-based authorization (ACL)
- Automatic WiFi reconnection
- Modular WiFi and MQTT managers
- Example periodic publish implementation
- Secrets isolation (`secrets.h` ignored from git)


# System Architecture
ESP32 → WiFi → TLS → HiveMQ Cloud Cluster

The device connects to a WiFi network, establishes a secure TLS connection to HiveMQ Cloud, authenticates using credentials, and publishes data to structured MQTT topics.


# Topic Structure

The project follows a scalable topic pattern:
devices/<device_id>/data
devices/<device_id>/cmd
devices/<device_id>/status

Example:
devices/dev001/data

This structure allows:

- Clear separation of device data
- Easy ACL management
- Multi-device scalability
- Future expansion for commands and telemetry


# Implementation Overview

## WiFi Layer

The ESP32 operates in **Station Mode (WIFI_MODE_STA)** and connects to a predefined network configured in `secrets.h`.

The WiFi manager:

- Initializes NVS
- Initializes TCP/IP stack (`esp_netif`)
- Registers WiFi and IP event handlers
- Connects automatically on startup
- Reconnects automatically if disconnected
- Tracks connection state using a FreeRTOS Event Group

This ensures MQTT communication only happens when network connectivity is available.


## Secure MQTT Layer

The project uses the ESP-IDF `esp-mqtt` component to connect securely to  
HiveMQ Cloud via:

- `mqtts://`
- Port `8883`
- TLS encryption
- Username/Password authentication
- Embedded CA certificate (ISRG Root X1)

The CA certificate is embedded into the firmware using:
EMBED_TXTFILES


This guarantees encrypted communication and broker certificate validation.


## Authentication & Authorization

Authentication is performed using credentials created in HiveMQ Cloud:

- Username
- Password

Authorization is controlled using Topic Filters (ACL), for example:
devices/#

Recommended production-level ACL:

This ensures each device can only access its own topic namespace.


## FreeRTOS Architecture

The MQTT client runs inside a dedicated FreeRTOS task.

The task:

1. Initializes the MQTT client
2. Registers event handlers
3. Starts the MQTT connection
4. Publishes a message every 5 seconds

This separation keeps the system modular, clean, and scalable.


# Project Structure
    main/
    ├── main.c
    ├── mqtt_manager.c
    ├── mqtt_manager.h
    ├── wifi_manager.c
    ├── wifi_manager.h
    ├── secrets.h (ignored in git)
    ├── secrets_example.h (template)
    ├── certs/
    │ └── isrg-root-x1-cross-signed.pem


# Security Model

## Authentication
- Username + Password created in HiveMQ Cloud
- Configured in `secrets.h`

## Authorization
- Topic-based permissions (ACL)
- Example:
devices/#


# What This Project Demonstrates

- Secure IoT communication over TLS
- Broker authentication and authorization
- Embedded certificate validation
- Scalable topic architecture
- Clean modular separation (WiFi + MQTT)
- Production-ready base template for ESP32 IoT projects
