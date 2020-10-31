[@react.component]
let make = () => {
  <ApolloClient.React.ApolloProvider client=Client.instance>
    <AdminOverview />
  </ApolloClient.React.ApolloProvider>;
};
