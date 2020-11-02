[%raw {|require("./styles/css/create-stream.css")|}];
[%raw {|require("./styles/css/global.css")|}];
[%raw {|require("./styles/css/defaults.css")|}];

open Belt;

[@decco.decode]
type animalDescription = array(string);

let (||||) = (optional, default) => Option.getWithDefault(optional, default);

module StringArray = {
  type t = array(string);
  let parse = json =>
    json->animalDescription_decode->Belt.Result.getWithDefault([||]);
  let serialize = array => array->Obj.magic->Js.Json.string;
};

module AllOrganisations = [%graphql
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

let customStyles = {
  "content": {
    "top": "20%",
    "left": "20%",
    "right": "20%",
    "bottom": "20%",
  },
  // "marginRight": "-20%",
  // "transform": "translate(-20%, -20%)",
};
type reactModalStyles;
module ReactModal = {
  [@bs.module "react-modal"] [@react.component]
  external make:
    (
      ~isOpen: bool,
      ~contentLabel: string,
      ~onAfterOpen: unit => unit,
      ~onRequestClose: unit => unit,
      ~style: reactModalStyles,
      ~children: React.element
    ) =>
    React.element =
    "default";
};
module CDNImageLink = {
  [@react.component]
  let make = (~cdnPath) => {
    let (showModal, setShowModal) = React.useState(_ => false);
    <div>
      <ReactModal
        isOpen=showModal
        contentLabel="hello world"
        style={customStyles->Obj.magic}
        onAfterOpen={() => ()}
        onRequestClose={() => setShowModal(_ => false)}>
        <img src={"https://dd2wadt5nc0o7.cloudfront.net" ++ cdnPath} />
      </ReactModal>
      <div onClick={_ => setShowModal(_ => true)}> cdnPath->React.string </div>
    </div>;
  };
};
module Description = {
  [@react.component]
  let make = (~description) => {
    let (showModal, setShowModal) = React.useState(_ => false);
    <div>
      <ReactModal
        isOpen=showModal
        contentLabel="hello world"
        style={customStyles->Obj.magic}
        onAfterOpen={() => ()}
        onRequestClose={() => setShowModal(_ => false)}>
        {description->Array.map(React.string)->React.array}
      </ReactModal>
      <div onClick={_ => setShowModal(_ => true)}>
        (
          switch (description->Array.length) {
          | 0 => "NO DESCRIPTION"
          | x => x->string_of_int ++ " paragraph description"
          }
        )
        ->React.string
      </div>
    </div>;
  };
};

module RealImages = {
  [@react.component]
  let make =
      (
        ~imageArray:
           array(
             AllOrganisations.AllOrganisations_inner.t_wildcardData_real_wc_photos,
           ),
      ) => {
    let (showModal, setShowModal) = React.useState(_ => false);
    <div>
      <ReactModal
        isOpen=showModal
        contentLabel="hello world"
        style={customStyles->Obj.magic}
        onAfterOpen={() => ()}
        onRequestClose={() => setShowModal(_ => false)}>
        {imageArray
         ->Array.map(({photographer, image}) =>
             <>
               <img src={"https://dd2wadt5nc0o7.cloudfront.net" ++ image} />
               {photographer->Option.mapWithDefault(React.null, React.string)}
             </>
           )
         ->React.array}
      </ReactModal>
      <div onClick={_ => setShowModal(_ => true)}>
        (
          switch (imageArray->Array.length) {
          | 0 => "NO IMAGES"
          | x => x->string_of_int ++ " real images"
          }
        )
        ->React.string
      </div>
    </div>;
  };
};
module YoutubeModal = {
  [@react.component]
  let make = (~optVideoCode) => {
    let (showModal, setShowModal) = React.useState(_ => false);
    <div>
      {switch (optVideoCode) {
       | Some(videoCode) =>
         <>
           <ReactModal
             isOpen=showModal
             contentLabel="hello world"
             style={customStyles->Obj.magic}
             onAfterOpen={() => ()}
             onRequestClose={() => setShowModal(_ => false)}>
             <Youtube videoCode />
           </ReactModal>
           <div onClick={_ => setShowModal(_ => true)}>
             videoCode->React.string
           </div>
         </>
       | None => "NO VIDEO"->React.string
       }}
    </div>;
  };
};

module Onboarding = {
  [@react.component]
  let make = (~onboarding_status) => {
    <div>
      ""->React.string
      (
        switch (onboarding_status) {
        | `contacted => "Contacted"
        | `live => "Live"
        | `rejected => "Rejected"
        | `listed => "Listed"
        | `unprocessed => "Unprocessed"
        | `declined => "Declined"
        | `interested => "Interested"
        | `signed => "Signed "
        | _ => "Unknown - please update ui"
        }
      )
      ->React.string
    </div>;
  };
};
[@react.component]
let make = () => {
  let allOrgsQuery = AllOrganisations.use();

  let onClick = (id, _event) => {
    Js.log("clicked the id: " ++ id);
  };

  <>
    <h2> "TODO: add filters."->React.string </h2>
    {switch (allOrgsQuery) {
     | {loading: true} => "Loading..."->React.string
     | {error: Some(_error)} =>
       Js.log(_error);
       "Error loading data"->React.string;
     | {data: Some({wildcardData})} =>
       <table>
         <thead>
           <tr>
             <th>
               {js| 🔑/#️⃣ |js}->React.string
               " key"->React.string
             </th>
             <th> {js| 🦒 |js}->React.string " name"->React.string </th>
             <th>
               {js| 🦇 |js}->React.string
               " common name"->React.string
             </th>
             <th> {js| 🦥 |js}->React.string " species"->React.string </th>
             <th>
               {js| 📋 |js}->React.string
               " description"->React.string
             </th>
             <th> {js| 🏷️ |js}->React.string " image"->React.string </th>
             <th>
               {js| 🧑‍🎨 |js}->React.string
               " artist"->React.string
             </th>
             <th>
               {js| 📸 |js}->React.string
               " real images"->React.string
             </th>
             <th> {js| 🏛️ |js}->React.string " org"->React.string </th>
           </tr>
         </thead>
         <tbody>
           {wildcardData
            ->Belt.Array.map(wildcard => {
                let {
                  nftId,
                  key,
                  name,
                  commonName,
                  species,
                  description,
                  image,
                  artistOfWildcard,
                  real_wc_photos,
                  organization,
                } = wildcard;
                let keyStr = key->string_of_int;

                <tr key=keyStr>
                  <td onClick={onClick(keyStr)}>
                    keyStr->React.string
                    {nftId->Option.mapWithDefault(React.null, id =>
                       <>
                         " (id: "->React.string
                         <a href={"https://wildcards.world/#details/" ++ id}>
                           id->React.string
                         </a>
                         ")"->React.string
                       </>
                     )}
                  </td>
                  <td onClick={onClick(keyStr)}>
                    {(name |||| "")->React.string}
                  </td>
                  <td onClick={onClick(keyStr)}>
                    {(commonName |||| "")->React.string}
                  </td>
                  <td onClick={onClick(keyStr)}>
                    {(species |||| "")->React.string}
                  </td>
                  <td> <Description description /> </td>
                  <td>
                    {image->Option.mapWithDefault(
                       "NO IMAGE"->React.string, coverImage =>
                       <CDNImageLink cdnPath=coverImage />
                     )}
                  </td>
                  <td>
                    {artistOfWildcard->Option.mapWithDefault(
                       "-"->React.string, ({name, id, eth_address, website}) =>
                       <>
                         name->React.string
                         {website->Option.mapWithDefault(React.null, website =>
                            <a href=website>
                              <small> website->React.string </small>
                            </a>
                          )}
                         {eth_address->Option.mapWithDefault(
                            React.null, ethAddress =>
                            <>
                              "EthAddr: "->React.string
                              <a
                                href={
                                  "https://etherscan.io/address/" ++ ethAddress
                                }>
                                <small> ethAddress->React.string </small>
                              </a>
                            </>
                          )}
                       </>
                     )}
                  </td>
                  <td> <RealImages imageArray=real_wc_photos /> </td>
                  <td onClick={onClick(keyStr)}>
                    {organization->Option.mapWithDefault(
                       React.null, ({name, id}) =>
                       <a href={"https://wildcards.world/#org/" ++ id}>
                         <small> name->React.string </small>
                       </a>
                     )}
                  </td>
                </tr>;
              })
            ->React.array}
         </tbody>
       </table>
     | _ => React.null
     }}
  </>;
};
