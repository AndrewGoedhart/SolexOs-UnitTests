#include <SolexOs/Assert.hpp>
#include <system_error>

extern "C" {
  /**
   * Global function to make logging errors easier.
   * Uses the singleton instance to shut down safely and do the actual logging.
   * Generally should be called through an ASSERT_xxx function. to minimise parameter overhead.
   *
   * \param recoveryStrategy what strategies the system is allowed to follow once it has moved to safe state to
   *                         resolve the issue.
   * \param error  the enumerated error that has occurred. Used to determine the issue when debugging the system from the logs
   * \param loginfo a 32 bit opaque value determined by the caller and useful for logging and analysis
   */
  void logError(const Assert::FatalRecoveryOption, const Assert::FatalErrorCode, const uint32_t ) {
    throw std::system_error{};
  }
}


