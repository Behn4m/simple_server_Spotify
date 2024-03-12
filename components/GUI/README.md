# LVGL Interface Module

This module facilitates LVGL 

## Table of Contents

- [Introduction](#introduction)
- [File Structure](#file-structure)
- [Dependencies](#dependencies)
- [Features](#features)
- [API Reference](#api-reference)
- [How to use it](#how-to-use-it)



## GPIO instructuer 

Essentially, all we require are callback functions for GPIOs. This implies that within the LVGL handler, we simply obtain pointers to the GPIOs callback functions. Before utilizing GPIOs, we initialize them within this handler, and that's all there is to it.