open Belt;

[@react.component]
let make = (~description) => {
  let (showModal, setShowModal) = React.useState(_ => false);
  <>
    <ReactModal
      isOpen=showModal
      contentLabel="hello world"
      style={ReactModal.customStyles->Obj.magic}
      onAfterOpen={() => ()}
      onRequestClose={() => setShowModal(_ => false)}>
      {switch (description) {
       | [||] => "NO DESCRIPTION SET"->React.string
       | _ =>
         description
         ->Array.map(paragraph => <p> paragraph->React.string </p>)
         ->React.array
       }}
    </ReactModal>
    <span onClick={_ => setShowModal(_ => true)}>
      (
        switch (description->Array.length) {
        | 0 => "NO DESCRIPTION"
        | x => x->string_of_int ++ " paragraph description"
        }
      )
      ->React.string
    </span>
  </>;
};
