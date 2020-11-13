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

module AllWildcardsData = [%graphql
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

// Add Real Wildcard Image:
module AddRealWildcardImage = [%graphql
  {|
  mutation ($inp: [real_wc_photos_insert_input!]!) {
    insert_real_wc_photos(objects: $inp) {
      affected_rows
    }
  }
  |}
];

module AddWildcardAnimation = [%graphql
  {|
    mutation ($key: Int!, $image: String!){
      update_wildcardData_by_pk(pk_columns: {key: $key}, _set: {image: $image}) {
        image
      }
    }
  |}
];

type imageLoadState =
  | Set(string)
  | Loaded(string)
  | ErrorLoading(string)
  | Done
  | NotSet;

module AddRealImages = {
  [@react.component]
  let make = (~imageKey) => {
    let (mutate, result) = AddRealWildcardImage.use();

    let (imageStatus, setImageStatus) = React.useState(_ => NotSet);
    let (photographer, setPhotographer) = React.useState(_ => None);

    let (link, setLink) = React.useState(() => "");

    let onChange = (e: ReactEvent.Form.t): unit => {
      let value = e->ReactEvent.Form.target##value;
      setLink(value);
    };
    let onChangePhotographer = (e: ReactEvent.Form.t): unit => {
      let value = e->ReactEvent.Form.target##value;
      setPhotographer(_ => Some(value));
    };

    let setImageForm =
      <form
        onSubmit={event => {
          ReactEvent.Form.preventDefault(event);
          setImageStatus(_ => Set(link));
        }}>
        <label>
          {React.string("Image link (remember, it starts with \"/\"): ")}
        </label>
        <br />
        <input type_="text" name="auth_key" onChange />
        <br />
        <label> {React.string("Has a photographer?")} </label>
        <input
          type_="checkbox"
          name="has_hotographer"
          onChange={e => {
            let value = e->ReactEvent.Form.target##checked;
            Js.log2("The event", e);
            Js.log2("The value", value);
            setPhotographer(_ => value ? Some("") : None);
            ();
          }}
        />
        {switch (photographer) {
         | Some(_) =>
           <>
             <br />
             <label> {React.string("Photographer name: ")} </label>
             <input
               type_="text"
               name="auth_key"
               onChange=onChangePhotographer
             />
           </>
         | None => React.null
         }}
        <br />
        <button> "Submit(test image is correct)"->React.string </button>
      </form>;

    <div>
      <h3> "Add new image:"->React.string </h3>
      <AuthComponent>
        <>
          // {switch (imageStatus) {
          //  | NotSet => "Not set"
          //  | Set(imageLink) => "Set" ++ imageLink
          //  | Loaded(imageLink) => "Loaded" ++ imageLink
          //  | ErrorLoading(imageLink) => "Error" ++ imageLink
          //  | Done => "DONE"
          //  }}
          // ->React.string
          {switch (imageStatus) {
           | NotSet => setImageForm
           | Set(imageLink) =>
             <>
               <a
                 href={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 target="_blank">
                 "Link to image (use it to debug issues...)"->React.string
               </a>
               <img
                 src={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 onLoad={_ => setImageStatus(_ => Loaded(imageLink))}
                 onError={_ => setImageStatus(_ => ErrorLoading(imageLink))}
               />
             </>
           | Loaded(imageLink) =>
             <>
               <button
                 onClick={_ => {
                   mutate(
                     ~refetchQueries=[|
                       AllWildcardsData.refetchQueryDescription(),
                     |],
                     AddRealWildcardImage.makeVariables(
                       ~inp=[|
                         {
                           photographer,
                           image: Some(imageLink),
                           wildcardKey: Some(imageKey),
                         },
                       |],
                       (),
                     ),
                   )
                   ->ignore;
                   setImageStatus(_ => Done);
                 }}>
                 "Set the image"->React.string
               </button>
               <img
                 src={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 onLoad={_ => setImageStatus(_ => Loaded(imageLink))}
                 onError={_ => setImageStatus(_ => ErrorLoading(imageLink))}
               />
             </>
           | ErrorLoading(imageLink) =>
             <>
               <p>
                 "There was an error loading that image, please check the link is correct:"
                 ->React.string
               </p>
               <a
                 href={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 target="_blank">
                 "Link to image (use it to debug issues...)"->React.string
               </a>
               <br />
               <h3> "try again"->React.string </h3>
               setImageForm
             </>
           | Done => React.null
           }}
        </>
        {switch (result) {
         | {called: false} => React.null
         | {loading: true} => "Loading..."->React.string
         | {data: Some(_result), error: None} =>
           <>
             <p> {React.string("New image added!")} </p>
             <br />
             {switch (imageStatus) {
              | Done =>
                <button onClick={_ => setImageStatus(_ => NotSet)}>
                  "Add another image?"->React.string
                </button>
              | _ => React.null
              }}
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
};
module AddAnimation = {
  [@react.component]
  let make = (~imageKey) => {
    let (mutate, result) = AddWildcardAnimation.use();

    let (imageStatus, setImageStatus) = React.useState(_ => NotSet);
    let (artist, setArtist) = React.useState(_ => None);

    let (link, setLink) = React.useState(() => "");

    let onChange = (e: ReactEvent.Form.t): unit => {
      let value = e->ReactEvent.Form.target##value;
      setLink(value);
    };
    let onChangeArtist = (e: ReactEvent.Form.t): unit => {
      let value = e->ReactEvent.Form.target##value;
      setArtist(_ => Some(value));
    };

    let setImageForm =
      <form
        onSubmit={event => {
          ReactEvent.Form.preventDefault(event);
          setImageStatus(_ => Set(link));
        }}>
        <label>
          {React.string("Image link (remember, it starts with \"/\"): ")}
        </label>
        <br />
        <input type_="text" name="auth_key" onChange />
        <br />
        <>
          <br />
          <label>
            {React.string("TODO: select the artist of this avatar")}
          </label>
          <input
            type_="text"
            name="auth_key"
            disabled=true
            onChange=onChangeArtist
          />
        </>
        <br />
        <button> "Submit (test animation is correct)"->React.string </button>
      </form>;

    <div>
      <h3> "Add new wildcard animation:"->React.string </h3>
      <AuthComponent>
        <>
          {switch (imageStatus) {
           | NotSet => setImageForm
           | Set(imageLink) =>
             <>
               <a
                 href={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 target="_blank">
                 "Link to image (use it to debug issues...)"->React.string
               </a>
               <img
                 src={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 onLoad={_ => setImageStatus(_ => Loaded(imageLink))}
                 onError={_ => setImageStatus(_ => ErrorLoading(imageLink))}
               />
             </>
           | Loaded(imageLink) =>
             <>
               <button
                 onClick={_ => {
                   mutate(
                     ~refetchQueries=[|
                       AllWildcardsData.refetchQueryDescription(),
                     |],
                     AddWildcardAnimation.makeVariables(
                       ~image=imageLink,
                       ~key=imageKey,
                       (),
                     ),
                   )
                   ->ignore;
                   setImageStatus(_ => Done);
                 }}>
                 "Set the image"->React.string
               </button>
               <img
                 src={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 onLoad={_ => setImageStatus(_ => Loaded(imageLink))}
                 onError={_ => setImageStatus(_ => ErrorLoading(imageLink))}
               />
             </>
           | ErrorLoading(imageLink) =>
             <>
               <p>
                 "There was an error loading that image, please check the link is correct:"
                 ->React.string
               </p>
               <a
                 href={"https://dd2wadt5nc0o7.cloudfront.net" ++ imageLink}
                 target="_blank">
                 "Link to image (use it to debug issues...)"->React.string
               </a>
               <br />
               <h3> "try again"->React.string </h3>
               setImageForm
             </>
           | Done => React.null
           }}
        </>
        {switch (result) {
         | {called: false} => React.null
         | {loading: true} => "Loading..."->React.string
         | {data: Some(_result), error: None} =>
           <>
             <p> {React.string("New wildcard animation added!")} </p>
             <br />
             {switch (imageStatus) {
              | Done =>
                <p>
                  "the image should be updated (you may need to reload your browser - will fix in future versions)"
                  ->React.string
                </p>
              | _ => React.null
              }}
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
};

module RealImages = {
  [@react.component]
  let make =
      (
        ~imageArray:
           array(
             AllWildcardsData.AllWildcardsData_inner.t_wildcardData_real_wc_photos,
           ),
        ~imageKey,
      ) => {
    let (showModal, setShowModal) = React.useState(_ => false);
    <div>
      <ReactModal
        isOpen=showModal
        contentLabel="hello world"
        style={ReactModal.customStyles->Obj.magic}
        onAfterOpen={() => ()}
        onRequestClose={() => setShowModal(_ => false)}>
        <div>
          <div className=Css.(style([backgroundColor(rgb(252, 209, 156))]))>
            <AddRealImages imageKey />
          </div>
          <hr />
          <hr />
          <h1> "Images:"->React.string </h1>
          {imageArray
           ->Array.map(({photographer, image}) =>
               <>
                 <img src={"https://dd2wadt5nc0o7.cloudfront.net" ++ image} />
                 {photographer->Option.mapWithDefault(
                    React.null,
                    React.string,
                  )}
               </>
             )
           ->React.array}
        </div>
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
             style={ReactModal.customStyles->Obj.magic}
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
module AddCardsModal = {
  [@react.component]
  let make = () => {
    let (showModal, setShowModal) = React.useState(_ => false);

    <>
      <ReactModal
        isOpen=showModal
        contentLabel="hello world"
        // style={ReactModal.customStyles->Obj.magic}
        onAfterOpen={() => ()}
        onRequestClose={() => setShowModal(_ => false)}>
        <AddWildcards />
      </ReactModal>
      <button
        className=Css.(style([color(white)]))
        onClick={_ => setShowModal(_ => true)}>
        "Add new wildcards"->React.string
      </button>
    </>;
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
  let allOrgsQuery = AllWildcardsData.use();
  let (showFilters, setShowFilters) = React.useState(_ => false);
  let (showActions, setShowActions) = React.useState(_ => false);

  let onClick = (id, _event) => {
    Js.log("clicked the id: " ++ id);
  };

  <>
    <br />
    {showFilters
       ? <>
           <a onClick={_ => {setShowFilters(_ => false)}}>
             "Close Filters"->React.string
           </a>
           <h2>
             "TODO: add filters. - be patient, Jason will do it one day"
             ->React.string
           </h2>
         </>
       : <>
           <a onClick={_ => {setShowFilters(_ => true)}}>
             "Show Filters"->React.string
           </a>
         </>}
    <br />
    {showActions
       ? <>
           <a onClick={_ => {setShowActions(_ => false)}}>
             "Close Actions"->React.string
           </a>
           <br />
           <AddCardsModal />
         </>
       : <>
           <a onClick={_ => {setShowActions(_ => true)}}>
             "Show Actions"->React.string
           </a>
         </>}
    <br />
    {switch (allOrgsQuery) {
     | {loading: true} => "Loading..."->React.string
     | {error: Some(error)} =>
       <AuthComponent>
         {(
            "Error loading data - message: "
            ++ error.message
            ++ "\n\n(copy paste the message to Jason on slack, but try reload the page first)"
          )
          ->React.string}
       </AuthComponent>
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
                    <CDNImageLink
                      cdnPath=image
                      updateAnimation={<AddAnimation imageKey=key />}
                    />
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
                  <td>
                    <RealImages imageKey=key imageArray=real_wc_photos />
                  </td>
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
