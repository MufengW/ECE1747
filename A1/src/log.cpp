#include "log.h"
#include "global.h"

#ifdef DEBUG
void log_result(const Particle& p, double force) {
    char output_buffer[256];
    snprintf(output_buffer, sizeof(output_buffer),
             "%d %d force = %e",
             p.x,
             p.y,
             force);

    M_log(output_buffer);
}
#endif

void printParticleInfo() {
    for (size_t i = 0; i < g_data.particle_info_list.size(); ++i) {
        ParticleInfo p_info = g_data.particle_info_list[i];
        std::cout << " Particle " << p_info.id
            << " x = " << p_info.x << ", "
            << "y = " << p_info.y << ", "
            << "force = " << p_info.force << std::endl;
        std::cout.flush();
    }
}
