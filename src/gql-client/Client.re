let graphqlEndpoint = "api.wildcards.world/v1/graphql";
// let graphqlEndpoint = "https://api.wildcards.world/v1/graphql";

let headers = {
  "eth-signature": "0xc46e2d4c2182ef506c9be3bec642878d8226bf69446bed045da63c6e415a2e0425d222e7e9db5dd42b32a337a9ca6055f7ca43c14ac0541c745ccd26a2cb44901b",
  "eth-address": "0xCB6F1a8fdcF0D8e16109ef629E85fB7a7b19876b",
};

let httpLink =
  ApolloClient.Link.HttpLink.make(
    ~uri=_ => "https://" ++ graphqlEndpoint,
    ~headers=Obj.magic(headers),
    (),
  );

let instance =
  ApolloClient.(
    make(
      ~cache=Cache.InMemoryCache.make(),
      ~connectToDevTools=true,
      ~defaultOptions=
        DefaultOptions.make(
          ~mutate=
            DefaultMutateOptions.make(
              ~awaitRefetchQueries=true,
              ~errorPolicy=All,
              (),
            ),
          ~query=
            DefaultQueryOptions.make(
              ~fetchPolicy=NetworkOnly,
              ~errorPolicy=All,
              (),
            ),
          ~watchQuery=
            DefaultWatchQueryOptions.make(
              ~fetchPolicy=NetworkOnly,
              ~errorPolicy=All,
              (),
            ),
          (),
        ),
      ~link=httpLink,
      (),
    )
  );
