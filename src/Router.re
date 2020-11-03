// open Globals;

module Route = {
  // type subOrgPage = |None
  // | Youtube
  // | Video
  // |
  type urlState =
    | Orgs
    | Cards;

  let fromUrl = (url: ReasonReactRouter.url) => {
    switch (url.path) {
    | ["orgs"]
    | ["orgs", ""] => Orgs
    | ["cards"]
    | ["cards", ""]
    | _ => Cards
    };
  };

  let toString = route =>
    switch (route) {
    | Orgs => "/orgs"
    | Cards => "/cards"
    };
};

let useRouter = () => ReasonReactRouter.useUrl()->Route.fromUrl;

let onRouteClick = (page, event) =>
  if (!event->ReactEvent.Mouse.defaultPrevented
      && event->ReactEvent.Mouse.button == 0
      && !event->ReactEvent.Mouse.altKey
      && !event->ReactEvent.Mouse.ctrlKey
      && !event->ReactEvent.Mouse.metaKey
      && !event->ReactEvent.Mouse.shiftKey) {
    event->ReactEvent.Mouse.preventDefault;
    page->Route.toString->ReasonReactRouter.push;
  };
