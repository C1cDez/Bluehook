#pragma once

// Assigned Numbers - https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf

// Assigned Numbers - 7.1 Company Identifiers by Value - p 218
const char* manufacturer(unsigned man);

// https://www.ampedrftech.com/datasheets/cod_definition.pdf
// Assigned Numbers - 2.8 Class of Device - p 46
void class_of_device_format(unsigned cod, char* buff, int size, const char* param_prefix);
