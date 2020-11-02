[@react.component]
let make = () => {
  <ApolloClient.React.ApolloProvider client=Client.instance>
    <AdminTopLevel />
  </ApolloClient.React.ApolloProvider>;
};
