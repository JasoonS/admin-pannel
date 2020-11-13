open Belt;

module AddWildcards = [%graphql
  {|
  mutation ($wildcardsToInsert: [wildcardData_insert_input!]!) {
  insert_wildcardData(objects: $wildcardsToInsert) {
    returning {
      key
      name
    }
  }
}
  |}
];

type index = int;
type name = string;
type inputActions =
  | AddRow
  | SetName(index, name)
  | SetSpecies(index, name)
  | SetCommonName(index, name);

type defaultState = {
  data: array(AddWildcards.t_variables_wildcardData_insert_input),
  isCurrentRowValid: bool,
};

// Some({
//   data: {
//     eth_address: Some(""),
//     id: Some("test"),
//     name: Some("person"),
//     website: None,
//   },
//   on_conflict:
//     Some({constraint_: `artist_pkey, update_columns: [||], where: None}),
// }),
type wildcardInput = AddWildcards.t_variables_wildcardData_insert_input;
let emptyNewCard: AddWildcards.t_variables_wildcardData_insert_input = {
  artist: None,
  artistOfWildcard: None,
  commonName: None,
  description: None,
  id: None,
  key: None,
  image: None,
  name: None,
  organisationId: None,
  organization: None,
  real_wc_photos: None,
  species: None,
  deprecated_id: None,
  nftMetadata: None,
  imageIpfsHash: None,
  ipfsMetadataHash: None,
};

type topLevelWildcardFields =
  | Artist
  | CommonName
  | Description
  | Image
  | Name
  | Organization
  | RealWcPhotos
  | Species;

module SetStringField = {
  [@react.component]
  let make = (~lable, ~currentValue, ~setValue, ~close) =>
    <>
      <label> lable->React.string </label>
      <br />
      <input
        type_="text"
        value=currentValue
        onChange={e => {
          let value = e->ReactEvent.Form.target##value;
          setValue(value);
        }}
      />
      <br />
      <br />
      <button className=Css.(style([color(white)])) onClick=close>
        "Finish"->React.string
      </button>
    </>;
};
module AddCardRow = {
  [@react.component]
  let make = (~rowData: wildcardInput, ~index, ~reducer) => {
    let {
      artist,
      artistOfWildcard,
      commonName,
      description,
      id,
      key,
      image,
      name,
      organisationId,
      organization,
      real_wc_photos,
      species,
    } = rowData;
    let (editing, setEditing) = React.useState(_ => None);
    let onClick = (editType, _event) => {
      setEditing(_ => Some(editType));
    };
    <>
      <tr>
        <td onClick={onClick(Name)}>
          {name->Option.getWithDefault("")->React.string}
        </td>
        <td onClick={onClick(CommonName)}>
          {commonName->Option.getWithDefault("")->React.string}
        </td>
        <td onClick={onClick(Species)}>
          {species->Option.getWithDefault("")->React.string}
        </td>
        <td onClick={onClick(Description)}>
          <Description
            description={description->Obj.magic->Option.getWithDefault([||])}
          />
        </td>
        <td onClick={onClick(Image)}>
          {image->Option.getWithDefault("")->React.string}
        </td>
        <td onClick={onClick(Artist)}>
          {artistOfWildcard->Option.mapWithDefault(
             "-"->React.string, ({data: {name, id, eth_address, website}}) =>
             <>
               {name->Option.getWithDefault("")->React.string}
               {website->Option.mapWithDefault(React.null, website =>
                  <a href=website> <small> website->React.string </small> </a>
                )}
               {eth_address->Option.mapWithDefault(React.null, ethAddress =>
                  <>
                    "EthAddr: "->React.string
                    <a href={"https://etherscan.io/address/" ++ ethAddress}>
                      <small> ethAddress->React.string </small>
                    </a>
                  </>
                )}
             </>
           )}
        </td>
        <td onClick={onClick(RealWcPhotos)}> "todo"->React.string </td>
        // {real_wc_photos
        //  ->Option.getWithDefault([||])
        //  ->Belt.Array.map(wildcardData =>
        //      <AddCardRow rowData=wildcardData />
        //    )
        //  ->React.array}
        <td onClick={onClick(Organization)}>
          {organization->Option.mapWithDefault(
             React.null, ({data: {name, id}}) =>
             <a
               href={
                 "https://wildcards.world/#org/"
                 ++ id->Option.getWithDefault("")
               }>
               <small>
                 {name->Option.getWithDefault("")->React.string}
               </small>
             </a>
           )}
        </td>
      </tr>
      {switch (editing) {
       | None => React.null
       | Some(thingToEdit) =>
         switch (thingToEdit) {
         | Artist =>
           <> "Adding artists isn't implemented yet"->React.string </>
         | Name =>
           <SetStringField
             lable="Set the name of the wildcard"
             currentValue={name->Option.getWithDefault("")}
             setValue={newValue => reducer(SetName(index, newValue))}
             close={_ => setEditing(_ => None)}
           />
         | CommonName =>
           <SetStringField
             lable="Set the common animal name of the wildcard"
             currentValue={commonName->Option.getWithDefault("")}
             setValue={newValue => reducer(SetCommonName(index, newValue))}
             close={_ => setEditing(_ => None)}
           />
         | Species =>
           <SetStringField
             lable="Set the species of the wildcard"
             currentValue={species->Option.getWithDefault("")}
             setValue={newValue => reducer(SetSpecies(index, newValue))}
             close={_ => setEditing(_ => None)}
           />
         | Description
         | Image
         | Organization
         | RealWcPhotos => React.null
         }
       }}
    </>;
  };
};

[@react.component]
let make = () => {
  let (mutate, result) = AddWildcards.use();

  let (state, reducer) =
    React.useReducer(
      (state, action) => {
        switch (action) {
        | SetName(index, name) =>
          state.data
          ->Array.set(
              index,
              {
                ...state.data[index]->Option.getWithDefault(emptyNewCard),
                name: Some(name),
              },
            )
          ->ignore;
          {...state, data: state.data};
        | SetCommonName(index, name) =>
          state.data
          ->Array.set(
              index,
              {
                ...state.data[index]->Option.getWithDefault(emptyNewCard),
                commonName: Some(name),
              },
            )
          ->ignore;
          {...state, data: state.data};
        | SetSpecies(index, name) =>
          state.data
          ->Array.set(
              index,
              {
                ...state.data[index]->Option.getWithDefault(emptyNewCard),
                species: Some(name),
              },
            )
          ->ignore;
          {...state, data: state.data};

        | AddRow => {
            ...state,
            data: state.data->Array.concat([|emptyNewCard|]),
          }
        }
      },
      {data: [|emptyNewCard|], isCurrentRowValid: false},
    );

  <div>
    <h3>
      "Add new wildcards: (this feature doesn't work yet)"->React.string
    </h3>
    <AuthComponent>
      {switch (result) {
       | {called: false} =>
         <>
           <table>
             <thead>
               <tr>
                 <th> {js| 🦒 |js}->React.string " name"->React.string </th>
                 <th>
                   {js| 🦇 |js}->React.string
                   " common name"->React.string
                 </th>
                 <th>
                   {js| 🦥 |js}->React.string
                   " species"->React.string
                 </th>
                 <th>
                   {js| 📋 |js}->React.string
                   " description"->React.string
                 </th>
                 <th>
                   {js| 🏷️ |js}->React.string
                   " image"->React.string
                 </th>
                 <th>
                   {js| 🧑‍🎨 |js}->React.string
                   " artist"->React.string
                 </th>
                 <th>
                   {js| 📸 |js}->React.string
                   " real images"->React.string
                 </th>
                 <th>
                   {js| 🏛️ |js}->React.string
                   " org"->React.string
                 </th>
               </tr>
             </thead>
             <tbody>
               {state.data
                ->Belt.Array.mapWithIndex((index, wildcardData) =>
                    <AddCardRow rowData=wildcardData index reducer />
                  )
                ->React.array}
             </tbody>
           </table>
           <button onClick={_ => reducer(AddRow)}>
             "+ Add row"->React.string
           </button>
         </>
       | {loading: true} => "Loading..."->React.string
       | {data: Some(_result), error: None} =>
         <>
           <p> {React.string("New wildcard animation added!")} </p>
           <br />
           <p> "Something"->React.string </p>
         </>
       | {error} =>
         <>
           "Error loading data"->React.string
           {switch (error) {
            | Some(error) => React.string(": " ++ error.message)
            | None => React.null
            }}
         </>
       }}
    </AuthComponent>
  </div>;
};
