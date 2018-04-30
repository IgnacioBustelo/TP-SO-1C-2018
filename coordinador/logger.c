#include <stdbool.h>
#include <commons/log.h>

t_log *logger;

/* Executed before main() */
__attribute__((constructor)) void init_log(void)
{
	logger = log_create("coordinador.log", "Coordinador", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger.");
}

/* Executed after main() */
__attribute__((destructor)) void destroy_log(void) {
	log_info(logger, "Finalizo la ejecucion del Coordinador.");
	log_destroy(logger);
}
