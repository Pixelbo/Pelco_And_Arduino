# Pelco and arduino

> Photography has nothing to do with cameras.

![A Pelco Camera](the%20eye%20of%20sauron.jpg)

## Overview

Hello, this is an Arduino library designed to control Pelco Cameras!

For now it can support **PELCO-D** protocol but I havn't got tested everything yet.

This library was made for the project: [Controlling a cctv camera on Arduino](https://hackaday.io/project/183986-controlling-a-cctv-camera-with-arduino) on hacakday.io

*Before you go on, this is a school, project, I'm still in school and learnings things so be kind if the libray is ugly. I would like to thanks my school and teachers for allowing me to work on this project.*

The main goal of the project is to do a control table (It'll be on arduino.)

Anyway, here the table of contents:

## Table of Contents
- [Pelco and arduino](#pelco-and-arduino)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Requirements](#requirements)
  - [Exemple](#exemple)


## Requirements

This is a bit obvious but you'll need a Pelco camera, the one I use the ES4036.

The important thing is to set the camera is RS485 biderectional and with the baud of your choosing. For now, the lib only support Pelco-D protocol.

About the modules for RS485, I'm working with a standart MAX485 chip but I'll test a more advanced module on the future (a seed-groove max485 module).

Tested on Arduino UNO and ESP32

## Exemple

go to the examples folder
