/* Open the Css module, so we can access the style properties below without prefixing them with Css. */
[@ocaml.warning "-44"]
open Css;

let buttonStyles =
  style([
    important(color(white)),
    textTransform(`uppercase),
    textDecoration(`none),
    background(`rgb((13, 42, 94))),
    padding(px(20)),
    borderRadius(px(5)),
    display(`inlineBlock),
    border(px(0), `none, white),
    transition(~duration=400, ~delay=0, ~timingFunction=ease, "all"),
    hover([
      background(`hex("434343")),
      letterSpacing(px(1)),
      boxShadow(
        Shadow.box(
          ~y=px(5),
          ~x=px(0),
          ~blur=px(5),
          rgba(0, 0, 0, `num(0.57)),
        ),
      ),
    ]),
  ]);
