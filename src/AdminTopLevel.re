[@react.component]
let make = () => {
  let route = Router.useRouter();

  <div>
    <button
      className=Styles.buttonStyles onClick={Router.onRouteClick(Cards)}>
      "Cards"->React.string
    </button>
    <button className=Styles.buttonStyles onClick={Router.onRouteClick(Orgs)}>
      "Orgs"->React.string
    </button>
    {switch (route) {
     | Orgs => <OrgOverview />
     | Cards => <CardOverview />
     }}
  </div>;
};
