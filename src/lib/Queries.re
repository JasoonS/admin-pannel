module AllWildcardsData = [%graphql
  {|
    query AllCards {
      wildcardData {
        commonName
        artistOfWildcard {
          name
          id
          eth_address
          website
        }
        nftId: id
        description @ppxCustom(module: "GqlConverters.StringArray")
        image
        key
        name
        species
        real_wc_photos {
          photographer
          image
        }
        organization {
          id
          name
        }
      }
    }
  |}
];
