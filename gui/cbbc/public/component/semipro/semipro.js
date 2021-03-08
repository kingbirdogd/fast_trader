class Semipro extends React.Component {
  constructor(props) {
    super(props)
    this.setStates = this.setStates.bind(this)
    this.getStates = this.getStates.bind(this)
    
    this.state = {}
    this.state.visible = {trade: true, position: false, order: false, setting: false}
  }
  
  componentDidMount() {

  }
  
  setStates(states) {this.setState(states)}
  
  getStates() {return this.state}
  
  render() {
    return(
      <React.Fragment>
      
        <Control
          key="control"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />
        
        {this.state.visible.trade &&
        <Trade
          key="trade"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />}
        
        {this.state.visible.position &&
        <Cash
          key="cash"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />}
        
        {this.state.visible.position &&
        <Position
          key="position"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />}
        
        {this.state.visible.order &&
        <Order
          key="order"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />}
        
        {this.state.visible.setting &&
        <Setting
          key="setting"
          lang={this.props.lang}
          setStates={this.setStates}
          getStates={this.getStates}
        />}
      
      </React.Fragment>
    )
  }
}

ReactDOM.render(
  <Semipro 
    lang={global.cookies[lang]}
    userId={userId}
  />, 
document.getElementById('semipro'))