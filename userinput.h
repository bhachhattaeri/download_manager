#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

void storeCustomDestinations(char* line);

void loadURL(char* line);

int checkHeading(char* line, int* category);

int setOptions();

void initialize();

void update();

void download(char* fileURL);

void downloadFromFile(char* fileName);

void cleanLine(char* line);

void run_program(char* flag, char* argument);

void print_help();

void err_opening_config_file();

void err_file_not_found();

void err_downloading_from_url();
