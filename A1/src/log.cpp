#include "log.h"
#include "global.h"
#include "parallel.h"

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

/*
 * Report the execution time and other details of the program.
 */
void report_time() {
#ifdef USE_MPI
    if (g_config.mode == 3) {
        std::cout << "Process #" << g_config.world_rank << std::endl;
    }
#endif

    std::cout << "Mode " << g_config.mode << " with " << g_config.thread_count <<
        (g_config.thread_count > 1 ? " threads " : " thread ")
        << "executed in " <<  g_data.duration.count() << " microseconds for "
        << g_config.particle_limit << " particles." << std::endl;
    M_log("Mode%d executed in %ld microseconds", g_config.mode, g_data.duration.count());
}

/*
 * Report the results of the computation.
 * Uses different reporting mechanisms based on the mode.
 */
void report_result() {
#ifdef USE_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
    if (g_config.mode != 3) {
        report_time();
        printParticleInfo();
    } else {
#ifdef USE_MPI
        synchronized_execution(report_time);
        MPI_Barrier(MPI_COMM_WORLD);
        synchronized_execution(printParticleInfo);
#endif
    }
}
