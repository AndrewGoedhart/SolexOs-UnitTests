#include <gtest/gtest.h>
#include <tasks/plcNetwork/Route.hpp>

namespace SolexOs {

  TEST(RouteTest, testCreate) {
    Route route = Route();
    route.addHop(NodeId::MASTER(), RxQuality(0x90));
    for( NodeId node = NodeId::FIRST_SLAVE(); node < NodeId(15); ++node ) {
      route.addHop(node, RxQuality(0x90));
    }
    NodeId expected[] = {NodeId::MASTER(), 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int i = 0;
    for( auto hop : route.hops() ) {
      EXPECT_EQ(hop.node, expected[i]);
      i++;
    }
  }

}

