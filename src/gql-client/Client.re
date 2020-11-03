// let graphqlEndpoint = "goerli.api.wildcards.world/v1/graphql";
let graphqlEndpoint = "api.wildcards.world/v1/graphql";
// let graphqlEndpoint = "https://api.wildcards.world/v1/graphql";

let headers = {
  let optAdminSecret =
    Dom.Storage.(localStorage |> getItem("hasura-admin-secret"));
  switch (optAdminSecret) {
  | Some(adminSecret) => {"x-hasura-admin-secret": adminSecret}
  // | None => {"no-auth": "nada"}->Obj.magic
  | None => {"x-hasura-admin-secret": "n[I>OWsXsUw9hG8^pGBU"}
  };
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
