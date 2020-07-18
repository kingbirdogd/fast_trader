class BetSize extends React.Component {
  static propTypes = {
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleAction = this.handleAction.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleAction() {
    var name = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    console.log(name, userId, algoName)
  }
  
  getText(lang) {
    var text = {
      en: {small: 'Small', oneLot: '1 Lot'},
      sc: {small: '细档', oneLot: '1手'},
      tc: {small: '細檔', oneLot: '1手'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    return(
      <div className='row'>
        <div className="col-12 col-sm-6 col-md-3 mb-3">
          <button
            name="BetSmall"
            type="button"
            className="btn btn-sm btn-secondary"
            onClick={this.handleAction}>
              {text.small}
          </button>
          <button
            name="Bet1Lot"
            type="button"
            className="btn btn-sm btn-secondary"
            onClick={this.handleAction}>
              {text.oneLot}
          </button>
        </div>
      </div>
    )
  }
}