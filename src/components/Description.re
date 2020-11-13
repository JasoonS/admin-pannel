open Belt;

[@react.component]
let make = (~description) => {
  let (showModal, setShowModal) = React.useState(_ => false);
  <div>
    <ReactModal
      isOpen=showModal
      contentLabel="hello world"
      style={ReactModal.customStyles->Obj.magic}
      onAfterOpen={() => ()}
      onRequestClose={() => setShowModal(_ => false)}>
      {description
       ->Array.map(paragraph => <p> paragraph->React.string </p>)
       ->React.array}
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
