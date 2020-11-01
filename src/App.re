[@react.component]
let make = () => {
  <ApolloClient.React.ApolloProvider client=Client.instance>
    <OrgOverview />
  </ApolloClient.React.ApolloProvider>;
};
