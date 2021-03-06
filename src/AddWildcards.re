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
module AllOrgIds = [%graphql
  {|
    query MyQuery {
      organisations {
        id
      }
    }
  |}
];

type index = int;
type name = string;
type orgId = string;
type paragraph = string;

type inputActions =
  | AddRow
  | SetName(index, name)
  | SetSpecies(index, name)
  | SetCommonName(index, name)
  | SetOrganisation(index, orgId)
  | SetDescription(index, array(paragraph));

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

[@bs.send] external join: (array(string), string) => string = "join";

let stringToParagraphs =
  Debouncer.make(((input: string, updateFunction)) => {
    // TODO: prevent adding strings with special characters
    let includesSpecialInvertedComasOrAppostrophes =
      input->Js.String2.includes("“")
      || input->Js.String2.includes("”")
      || input->Js.String2.includes("‘")
      || input->Js.String2.includes("’");
    let lines = input->Js.String2.split("\n");
    let trimmedLines = lines->Array.map(Js.String.trim);
    let paragraphs = trimmedLines->Array.keep(paragraph => paragraph != "");
    updateFunction(paragraphs);
  });

module SetParagarphArrayField = {
  [@react.component]
  let make = (~lable, ~currentValue: array(string), ~setValue, ~close) => {
    let (textareaValue, setTextareaValue) =
      React.useState(_ => currentValue->join("\n\n"));
    Js.log(textareaValue);
    React.useEffect1(
      () => {
        stringToParagraphs((textareaValue, setValue));
        None;
      },
      [|textareaValue|],
    );

    <>
      <td colSpan=4>
        <label> lable->React.string </label>
        <br />
        <textarea
          value=textareaValue
          className=Css.(style([width(`percent(100.)), height(em(25.))]))
          onChange={e => {
            let value = e->ReactEvent.Form.target##value;

            setTextareaValue(value);
          }}
        />
        <br />
        <br />
        <button className=Css.(style([color(white)])) onClick=close>
          "Save and continue"->React.string
        </button>
      </td>
      <td colSpan=4>
        {currentValue
         ->Array.mapWithIndex((index, paragraph) =>
             <>
               <small className=Css.(style([color(grey)]))>
                 {("Paragraph #" ++ index->string_of_int)->React.string}
               </small>
               <p> paragraph->React.string </p>
             </>
           )
         ->React.array}
      </td>
    </>;
  };
};
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
module SelectOrganisation = {
  [@react.component]
  let make = (~lable, ~currentValue, ~setValue, ~close) => {
    let allOrgsQuery = AllOrgIds.use();

    <>
      {switch (allOrgsQuery) {
       | {loading: true} =>
         "Loading list of all organisations..."->React.string
       | {error: Some(error)} =>
         <AuthComponent>
           {(
              "Error loading data - message: "
              ++ error.message
              ++ "\n\n(copy paste the message to Jason on slack, but try reload the page first)"
            )
            ->React.string}
         </AuthComponent>
       | {data: Some({organisations})} =>
         <>
           <label> lable->React.string </label>
           <br />
           <br />
           <select
             value=currentValue
             onChange={e => {
               let value = e->ReactEvent.Form.target##value;

               setValue(value);
             }}>
             {organisations
              ->Array.map(({id}) =>
                  <option value=id> id->React.string </option>
                )
              ->React.array}
           </select>
           <br />
           <button className=Css.(style([color(white)])) onClick=close>
             "Finish"->React.string
           </button>
         </>
       | _ => React.null
       }}
    </>;
  };
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
          <div>
            <Description
              description={description->Obj.magic->Option.getWithDefault([||])}
            />
            <h1
              onClick={onClick(Description)}
              className=Css.(style([display(`inline)]))>
              {js| 🗒️✍ |js}->React.string
            </h1>
          </div>
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
          {organisationId->Option.mapWithDefault(React.null, orgId =>
             <>
               <a href={"https://wildcards.world/#org/" ++ orgId}>
                 <small> orgId->React.string </small>
               </a>
               {js| 🗒️✍ |js}->React.string
             </>
           )}
        </td>
      </tr>
      <tr>
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
           | Description =>
             <SetParagarphArrayField
               lable="Edit description for the wildcard"
               currentValue={
                 description->Option.mapWithDefault(
                   [||],
                   GqlConverters.StringArray.parse,
                 )
               }
               setValue={newValue =>
                 reducer(SetDescription(index, newValue))
               }
               close={_ => setEditing(_ => None)}
             />
           | Organization =>
             <SelectOrganisation
               lable="Select the orginization for this wildcard"
               currentValue={organisationId->Option.getWithDefault("")}
               setValue={(newValue: string) =>
                 reducer(SetOrganisation(index, newValue))
               }
               close={_ => setEditing(_ => None)}
             />
           | Image
           | RealWcPhotos => React.null
           }
         }}
      </tr>
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
        | SetDescription(index, description) =>
          state.data
          ->Array.set(
              index,
              {
                ...state.data[index]->Option.getWithDefault(emptyNewCard),
                description:
                  description->Array.length > 0
                    ? Some(description->GqlConverters.StringArray.serialize)
                    : None,
              },
            )
          ->ignore;
          {...state, data: state.data};
        | SetOrganisation(index, organisationId) =>
          state.data
          ->Array.set(
              index,
              {
                ...state.data[index]->Option.getWithDefault(emptyNewCard),
                organisationId: Some(organisationId),
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
           <br />
           <br />
           <br />
           <br />
           {switch (state.data) {
            | [||] => React.null
            | wildcardsToInsert =>
              <button
                onClick={_ => {
                  mutate(
                    ~refetchQueries=[|
                      Queries.AllWildcardsData.refetchQueryDescription(),
                    |],
                    AddWildcards.makeVariables(~wildcardsToInsert, ()),
                  )
                  ->ignore
                }}>
                "Save Wildcards"->React.string
              </button>
            }}
         </>
       | {loading: true} => "Loading..."->React.string
       | {data: Some(_result), error: None} =>
         <>
           <p> {React.string("New wildcards have been added!")} </p>
           <br />
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
