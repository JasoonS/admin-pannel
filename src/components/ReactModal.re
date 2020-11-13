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

[@bs.module "react-modal"] [@react.component]
external make:
  (
    ~isOpen: bool,
    ~contentLabel: string,
    ~onAfterOpen: unit => unit,
    ~onRequestClose: unit => unit,
    ~style: reactModalStyles=?,
    ~children: React.element
  ) =>
  React.element =
  "default";
