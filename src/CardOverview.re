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
query AllOrgs {
  organisations {
    announcement_blog
    country
    cover_image
    description  @ppxCustom(module: "StringArray")
    id
    logo
    logo_badge
    name
    onboarding_status
    website
    youtube_vid
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
    <h2> "TODO: add filters"->React.string </h2>
    {switch (allOrgsQuery) {
     | {loading: true} => "Loading..."->React.string
     | {error: Some(_error)} =>
       Js.log(_error);
       "Error loading data"->React.string;
     | {data: Some({organisations})} =>
       <table>
         <thead>
           <tr>
             <th> {js| 📁 |js}->React.string " name"->React.string </th>
             <th> {js| 📥 |js}->React.string " id"->React.string </th>
             <th>
               {js| 🌍 |js}->React.string
               " contry code"->React.string
             </th>
             <th>
               {js| 🕔 |js}->React.string
               " logo image"->React.string
             </th>
             <th> {js| ⏳ |js}->React.string " logo badge"->React.string </th>
             <th>
               {js| 🧮 |js}->React.string
               " cover image"->React.string
             </th>
             <th>
               {js| 📋 |js}->React.string
               " description"->React.string
             </th>
             <th>
               {js| 🏷️ |js}->React.string
               " announcement blog"->React.string
             </th>
             <th> {js| 🌐 |js}->React.string " website"->React.string </th>
             <th>
               {js| 🎞 |js}->React.string
               " youtube vid"->React.string
             </th>
             <th> {js| 🏗️ |js}->React.string " status"->React.string </th>
           </tr>
         </thead>
         <tbody>
           {organisations
            ->Belt.Array.map(
                (
                  {
                    announcement_blog,
                    country,
                    cover_image,
                    description,
                    id,
                    logo,
                    logo_badge,
                    name,
                    onboarding_status,
                    website,
                    youtube_vid,
                  },
                ) => {
                Js.log(onboarding_status);
                <tr key=id>
                  <td onClick={onClick(id)}> name->React.string </td>
                  <td onClick={onClick(id)}> id->React.string </td>
                  <td onClick={onClick(id)}>
                    {(country |||| "")->React.string}
                  </td>
                  <td> <CDNImageLink cdnPath=logo /> </td>
                  <td>
                    {logo_badge->Option.mapWithDefault(
                       ""->React.string, logoBadge =>
                       <CDNImageLink cdnPath=logoBadge />
                     )}
                  </td>
                  <td>
                    {cover_image->Option.mapWithDefault(
                       ""->React.string, coverImage =>
                       <CDNImageLink cdnPath=coverImage />
                     )}
                  </td>
                  <td> <Description description /> </td>
                  <td onClick={onClick(id)}>
                    {(announcement_blog |||| "")->React.string}
                  </td>
                  <td onClick={onClick(id)}>
                    <a href=website target="_blank">
                      <small> website->React.string </small>
                    </a>
                  </td>
                  <td onClick={onClick(id)}>
                    <YoutubeModal optVideoCode=youtube_vid />
                  </td>
                  <td onClick={onClick(id)}>
                    <Onboarding onboarding_status />
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
