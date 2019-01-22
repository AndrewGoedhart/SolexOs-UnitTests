#include <gtest/gtest.h>
#include <tasks/plcNetwork/trackers/NetworkScan.hpp>
#include <tasks/plcNetwork/Route.hpp>

namespace SolexOs {

  constexpr Route createRoute(std::initializer_list<uint8_t> routeNodes){
    Route route;
    for( auto node :routeNodes){
      route.addHop(NodeId(node), RxQuality(80));
    }
    return route;
  }


  TEST(NetworkScanTest, testCreateBranched) {
    NodeSet requiredNodes;
    for( uint i=0; i < 15; i++){
      requiredNodes.add(NodeId(i+NodeId::FIRST_SLAVE().asInt()));
    }
    NodeSet failedNodes;
    NodeSet expectedRepeaters({8,7,14,11,16});
    NodeSet expectedAckNodes({18,19,20});


    NetworkScan scan(requiredNodes, failedNodes);

    // build route
    Route routes[] = { createRoute({3,7,16,20}),
                       createRoute({3,8,11,19}),
                       createRoute({3,8,14,18}),
                       createRoute({3,8,14,17}),
                       createRoute({3,7,16}),
                       createRoute({3,8,14,15}),
                       createRoute({3,7,13}),
                       createRoute({3,8,12}),
                       createRoute({3,8,10}),
                       createRoute({3,8,9}),
                       createRoute({3,6}) };

    for( auto route :routes){
      scan.addRoute(route);
      scan.log("");
      LOG("\r");
    };

   EXPECT_TRUE(scan.isScanComplete());
   EXPECT_EQ(scan.repeaters(), expectedRepeaters);
   EXPECT_EQ(scan.getAckNodes(), expectedAckNodes);

  }

  TEST(NetworkScanTest, testCreateLinear) {
    NodeSet requiredNodes;
    for( uint i=0; i < 4; i++){
      requiredNodes.add(NodeId(i+NodeId::FIRST_SLAVE().asInt()));
    }
    NodeSet failedNodes;
    NodeSet expectedRepeaters({6,7,8});
    NodeSet expectedAckNodes({9});


    NetworkScan scan(requiredNodes, failedNodes);

    // build route
    Route routes[] = { createRoute({3,6,7,8,9})};

    for( auto route :routes){
      scan.addRoute(route);
      scan.log("");
      LOG("\r");
    };

   EXPECT_TRUE(scan.isScanComplete());
   EXPECT_EQ(scan.repeaters(), expectedRepeaters);
   EXPECT_EQ(scan.getAckNodes(), expectedAckNodes);

  }

}

