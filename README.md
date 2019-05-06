# ESP8266-Google-Sheets-Data-Logging

## Introduction
This content was created by Chris Nguyen for BME 201 at University of Wisconsin Madison. Its implementation in Spring 2019, marked the first time the Introduction to BME Design course began to use wireless data logging. This change eliminated common hardware failures when data logging onto an SD card. It also allowed students and instructors the ability to monitor the process wirelessly and remotely. 

The files presented here can be easily modified to fit your needs. The provided sample code for the ESP8266 simply reads data in via the Serial interface and sends that data to Google. Server-side calculations and actions are performed within a Google Script (javascript like code that lives on the Google Server). 

An Arduino is not required - all circuitry can be completed on-board the NodeMCU. An Arduino was used for this class in order to have access to 5V

## Instructions
- Instructions on how to set up a Google Sheet to log data can be found in How_to_set-up_Google_Sheets.pdf
- Instructions on how to set up the ESP8266 can be found in How_to_set-up_ESP8266.pdf
- Instructions on how to communicate between an Arduino and the ESP8266 can be found in How_to_Communicate.pdf

## Example files
Example files are provided for:
- Google Sheet
- Google Script
- Arduino sketch
- NodeMCU (ESP8266) board
