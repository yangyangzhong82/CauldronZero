#include "ll/api/io/Logger.h"
#include "CauldronZero/Entry/Entry.h"
namespace CauldronZero  {
inline ll::io::Logger& logger = Entry::getInstance().getSelf().getLogger(); // logger.Trace Debug Info Warn Error Fatal
}