open Belt;

[@react.component]
let make = (~cdnPath, ~updateAnimation) => {
  let (showModal, setShowModal) = React.useState(_ => false);

  <div>
    <ReactModal
      isOpen=showModal
      contentLabel="hello world"
      style={ReactModal.customStyles->Obj.magic}
      onAfterOpen={() => ()}
      onRequestClose={() => setShowModal(_ => false)}>
      updateAnimation
      {cdnPath->Option.mapWithDefault(
         <p> "you need to upload an image still"->React.string </p>, imagePath =>
         <img src={"https://dd2wadt5nc0o7.cloudfront.net" ++ imagePath} />
       )}
    </ReactModal>
    <div onClick={_ => setShowModal(_ => true)}>
      {cdnPath->Option.getWithDefault("NO IMAGE")->React.string}
    </div>
  </div>;
};
