open Belt;

[@react.component]
let make = (~children) => {
  let (authSet, setAuthSet) =
    React.useState(_ =>
      Dom.Storage.(localStorage |> getItem("hasura-admin-secret"))
      ->Option.isSome
    );
  let (authHeader, setAuthHeader) = React.useState(() => "");

  let onChange = (e: ReactEvent.Form.t): unit => {
    let value = e->ReactEvent.Form.target##value;
    setAuthHeader(value);
  };

  authSet
    ? <>
        <button onClick={_ => setAuthSet(_ => false)}>
          "Edit your auth key"->React.string
        </button>
        <br />
        children
      </>
    : <form
        onSubmit={event => {
          Dom.Storage2.(
            localStorage->setItem("hasura-admin-secret", authHeader)
          );
          ReactEvent.Form.preventDefault(event);

          setAuthSet(_ => true);
        }}>
        <label> {React.string("Auth Key: ")} </label>
        <input type_="text" name="auth_key" onChange />
        <button type_="submit"> {React.string("submit")} </button>
        <br />
        <br />
        <br />
        <p>
          "NOTE: you may need to reload the webpage after doing this to activate the key"
          ->React.string
        </p>
      </form>;
};
