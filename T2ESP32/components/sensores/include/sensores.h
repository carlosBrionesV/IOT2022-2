
float floatrand(float min, float max);
int intrand(int min, int max);

float *acc_sensor_acc_x();
float *acc_sensor_acc_y();
float *acc_sensor_acc_z();
float *acc_sensor_rgyr_x();
float *acc_sensor_rgyr_y();
float *acc_sensor_rgyr_z();
int thpc_sensor_temp();
int thpc_sensor_hum();
int thpc_sensor_pres();
float thpc_sensor_co();
float batt_sensor();
float acc_kpi_amp_x();
float acc_kpi_frec_x();
float acc_kpi_amp_y();
float acc_kpi_frec_y();
float acc_kpi_amp_z();
float acc_kpi_frec_z();
float acc_kpi_rms(float x, float y, float z);